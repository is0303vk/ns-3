/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2018 Yuki Kitagawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Yuki Kitagawa <is0303vk@ed.ritsumei.ac.jp>
 *         Ritsumeikan University, Shiga, Japan
 */
#include "kitagawa-packet.h"
#include "ns3/address-utils.h"
#include "ns3/packet.h"
#include <cmath>

namespace ns3 {
namespace kitagawa {

NS_OBJECT_ENSURE_REGISTERED (TypeHeader);

TypeHeader::TypeHeader (MessageType t)
  : m_type (t),
    m_valid (true)
{
}

TypeId
TypeHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::kitagawa::TypeHeader")
    .SetParent<Header> ()
    .SetGroupName ("Kitagawa")
    .AddConstructor<TypeHeader> ()
  ;
  return tid;
}

TypeId
TypeHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
TypeHeader::GetSerializedSize () const
{
  return 1;
}

void
TypeHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8 ((uint8_t) m_type);
}

uint32_t
TypeHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  uint8_t type = i.ReadU8 ();
  m_valid = true;
  switch (type)
    {
    case BROADCASTTYPE:
    case BROADCASTTYPE2:
    case BROADCASTTYPE3:
    case UNICASTTYPE:
    case HELLOTYPE:
      {
        m_type = (MessageType) type;
        break;
      }
    default:
      m_valid = false;
    }
  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
TypeHeader::Print (std::ostream &os) const
{
  switch (m_type)
    {
    case BROADCASTTYPE:
      {
        os << "Broadcast(Emergency)";
        break;
      }
    case BROADCASTTYPE2:
      {
        os << "Broadcast2(RSU)";
        break;
      }
    case BROADCASTTYPE3:
      {
        os << "Broadcast3(car)";
        break;
      }
    case UNICASTTYPE:
      {
        os << "Unicast";
        break;
      }
    case HELLOTYPE:
      {
        os << "Hello";
        break;
      }
    default:
      os << "UNKNOWN_TYPE";
    }
}

bool
TypeHeader::operator== (TypeHeader const & o) const
{
  return (m_type == o.m_type && m_valid == o.m_valid);
}

std::ostream &
operator<< (std::ostream & os, TypeHeader const & h)
{
  h.Print (os);
  return os;
}

//-----------------------------------------------------------------------------
// Broadcast
//-----------------------------------------------------------------------------
BroadcastHeader::BroadcastHeader (uint8_t e_start, Time time,  uint16_t source, uint16_t e_dst_x, uint16_t e_dst_y, uint8_t instruction, uint16_t hopCount, uint8_t sequence) 
  //: m_hopCount (hopCount),
  //  m_origin (origin)
{
  e_time = uint32_t (time.GetMilliSeconds());
}


TypeId
BroadcastHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::kitagawa::BroadcastHeader")
    .SetParent<Header> ()
    .SetGroupName("Kitagawa")
    .AddConstructor<BroadcastHeader> ()
  ;
  return tid;
}

TypeId
BroadcastHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
BroadcastHeader::GetSerializedSize () const
{
  //8bits(start)+32bits(time)+16bits(source)+16bits(destination_x)+16bits(destination_y)+8bits(instruction)+16bits(hopCount)+8bits(sequence)
  return 15;
}

void
BroadcastHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8 (e_start);
  i.WriteHtonU32(e_time);
  i.WriteU16 (source);
  i.WriteU16 (e_dst_x);
  i.WriteU16 (e_dst_y);
  i.WriteU8 (instruction);
  i.WriteU16 (m_hopCount);
  i.WriteU8 (sequence);
}

uint32_t
BroadcastHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  
  e_start = i.ReadU8 ();
  e_time = i.ReadNtohU32();
  source = i.ReadU16 ();
  e_dst_x = i.ReadU16 ();
  e_dst_y = i.ReadU16 ();
  instruction = i.ReadU8 ();
  m_hopCount = i.ReadU16 ();
  sequence = i.ReadU8 ();
  

  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
BroadcastHeader::Print (std::ostream &os) const
{
  os <<" Emergency Start " << e_start
     <<" SendTime "<<e_time 
     <<" Source "<< source
     <<" X of Emergency Destination " << e_dst_x
     <<" Y of Emergency Destination " << e_dst_y
     <<" Instruction " << instruction
     <<" Hop Count " << m_hopCount
     <<" Sequence "<< sequence;
}    

std::ostream &
operator<<(std::ostream & os, BroadcastHeader const & h)
{
  h.Print(os);
  return os;
}

//-----------------------------------------------------------------------------
// Unicast
//-----------------------------------------------------------------------------
UnicastHeader::UnicastHeader (uint8_t e_start, Time time,  uint16_t source, uint16_t e_dst, uint8_t instruction, uint16_t hopCount) 
  //: m_hopCount (hopCount),
  //  m_origin (origin)
{
  e_time = uint32_t (time.GetMilliSeconds());
}


TypeId
UnicastHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::kitagawa::UnicastHeader")
    .SetParent<Header> ()
    .SetGroupName("Kitagawa")
    .AddConstructor<UnicastHeader> ()
  ;
  return tid;
}

TypeId
UnicastHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
UnicastHeader::GetSerializedSize () const
{
  //8bits(start)+32bits(time)+16bits(source)+16bits(destination)+8bits(instruction)+16bits(hopCount)
  //1*4 + 2 bytes 4bytes = 10 bytes
  return 13;
}

void
UnicastHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU8 (e_start);
  i.WriteHtonU32(e_time);
  i.WriteU16 (source);
  i.WriteU16 (e_dst);
  i.WriteU8 (instruction);
  i.WriteU16 (m_hopCount);
}

uint32_t
UnicastHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  
  e_start = i.ReadU8 ();
  e_time = i.ReadNtohU32();
  source = i.ReadU16 ();
  e_dst = i.ReadU16 ();
  instruction = i.ReadU8 ();
  m_hopCount = i.ReadU16 ();
  

  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
UnicastHeader::Print (std::ostream &os) const
{
  os <<" Emergency Start " << e_start
     <<" SendTime "<<e_time 
     <<" Source "<< source
     <<" Emergency Destination " << e_dst
     <<" Instruction " << instruction
     <<" Hop Count " << m_hopCount;
}    

std::ostream &
operator<<(std::ostream & os, UnicastHeader const & h)
{
  h.Print(os);
  return os;
}




//----------------------------------------------------------------
//HELLO
//----------------------------------------------------------------
HelloHeader::HelloHeader(uint16_t nodeId, uint16_t position_X, uint16_t position_Y)
  :m_nodeId(nodeId),
   m_position_X(position_X),
   m_position_Y(position_Y)
{
}

TypeId
HelloHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::kitagawa::HelloHeader")
    .SetParent<Header> ()
    .SetGroupName("Kitagawa")
    .AddConstructor<HelloHeader> ()
  ;
  return tid;
}

TypeId
HelloHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

uint32_t
HelloHeader::GetSerializedSize () const
{
  //16bits(nodeId)+16bits(position_X)+16bits(position_Y)
  //2bytes + 2bytes + 2bytes= 6bytes
  return 6;
}

void
HelloHeader::Serialize (Buffer::Iterator i) const
{
  i.WriteU16 (m_nodeId);
  i.WriteU16 (m_position_X);
  i.WriteU16 (m_position_Y);
}

uint32_t
HelloHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  
  m_nodeId = i.ReadU16 ();
  m_position_X = i.ReadU16 ();
  m_position_Y = i.ReadU16 ();
  

  uint32_t dist = i.GetDistanceFrom (start);
  NS_ASSERT (dist == GetSerializedSize ());
  return dist;
}

void
HelloHeader::Print (std::ostream &os) const
{
  os <<" nodeId " << m_nodeId
     <<" Position_X "<<m_position_X
     <<" Position_Y "<<m_position_Y;     
}    

std::ostream &
operator<<(std::ostream & os, HelloHeader const & h)
{
  h.Print(os);
  return os;
}

} //namespace kitagawa
} //namespace ns3
