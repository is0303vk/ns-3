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
#ifndef KITAGAWAPACKET_H
#define KITAGAWAPACKET_H

#include <iostream>
#include "ns3/header.h"
#include "ns3/enum.h"
#include "ns3/ipv4-address.h"
#include <map>
#include "ns3/nstime.h"

namespace ns3 {
namespace kitagawa {

/**
* \ingroup kitagawa
* \brief MessageType enumeration
*/
enum MessageType
{
  BROADCASTTYPE  = 1,   //!< BROADCASTTYPE
  BROADCASTTYPE2 = 2,   //!< BROADCASTTYPE2
  BROADCASTTYPE3 = 3,    //!< BROADCASTTYPE3
  UNICASTTYPE = 4,      //!< UNICASTTYPE
  HELLOTYPE = 5,        //!< HELLO
};

/**
* \ingroup kitagawa
* \brief KITAGAWA types
*/
class TypeHeader : public Header
{
public:
  /**
   * constructor
   * \param t the HELLOTYPE type
   */
  TypeHeader (MessageType t = BROADCASTTYPE);

  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId ();
  TypeId GetInstanceTypeId () const;
  uint32_t GetSerializedSize () const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void Print (std::ostream &os) const;

  /**
   * \returns the type
   */
  MessageType Get () const
  {
    return m_type;
  }
  /**
   * Check that type if valid
   * \returns true if the type is valid
   */
  bool IsValid () const
  {
    return m_valid;
  }
  /**
   * \brief Comparison operator
   * \param o header to compare
   * \return true if the headers are equal
   */
  bool operator== (TypeHeader const & o) const;
private:
  MessageType m_type; ///< type of the message
  bool m_valid; ///< Indicates if the message is valid
};

/**
  * \brief Stream output operator
  * \param os output stream
  * \return updated stream
  */
std::ostream & operator<< (std::ostream & os, TypeHeader const & h);

/**
* \ingroup kitagawa
* \brief   Broadcast (BROADCASTTYPE,BROADCASTTYPE2,BROADCASTTYPE3) Message Format
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                        緊急車両現在地ID                        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    緊急車両のパケット送信時刻                  　　|
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                          送信元ID                           　  |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                        緊急車両目的地座標X                     |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                        緊急車両目的地座標Y                     |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                          HopCount     　　　　　　　　　　　　　　　　　　　　　　　　|
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                            指示                           　   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                        シーケンス番号                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                       送信元ノードの種類                        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  
  \endverbatim
*/
class BroadcastHeader : public Header 
{
public:
  /// c-tor
  BroadcastHeader (uint16_t e_start = 0, Time e_time = MilliSeconds(0), uint16_t source = 0, uint32_t e_dst_x = 0, uint32_t e_dst_y = 0, uint16_t instruction = 0, uint16_t hopCount = 0, uint16_t sequence = 0,uint16_t source_type = 0);

  // Header serialization/deserialization
  static TypeId GetTypeId ();
  TypeId GetInstanceTypeId () const;
  uint32_t GetSerializedSize () const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void Print (std::ostream &os) const;

  // Fields
  void SetE_Start(uint16_t start)
  {
    e_start = start;
  }
  uint16_t GetE_Start() const
  {
    return e_start;
  }
  
  void SetTime(Time time)
  {
    e_time = time.GetMilliSeconds();
  }
  Time GetTime()
  {
    Time t(MilliSeconds(e_time));
    return t;
  }
   
  void SetSource(uint16_t mysource)
  {
    source = mysource;
  }
  uint16_t GetSource() const
  {
    return source;
  }

  void SetE_Dst_X(uint32_t destination_x)
  {
    e_dst_x = destination_x;
  }
  uint32_t GetE_Dst_X() const
  {
    return e_dst_x;
  }

  void SetE_Dst_Y(uint32_t destination_y)
  {
    e_dst_y = destination_y;
  }
  uint32_t GetE_Dst_Y() const
  {
    return e_dst_y;
  }

  void SetInstruction(uint16_t inst)
  {
    instruction = inst;
  }
  uint16_t GetInstruction() const
  {
    return instruction;
  }
  
  void SetHopCount (uint16_t count) 
  { 
    m_hopCount = count; 
  }
  uint16_t GetHopCount () const 
  { 
    return m_hopCount; 
  }

  void SetSequence (uint16_t number) 
  { 
    sequence = number; 
  }
  uint16_t GetSequence () const 
  { 
    return sequence; 
  }

  void SetSource_Type (uint16_t source_send_type) 
  { 
    source_type = source_send_type; 
  }
  uint16_t GetSource_Type () const 
  { 
    return source_type; 
  }


private:
  uint16_t e_start;
  uint32_t e_time;
  uint16_t source;
  uint32_t e_dst_x;
  uint32_t e_dst_y;
  uint16_t instruction;
  uint16_t m_hopCount;
  uint16_t sequence;
  uint16_t source_type;
};

std::ostream & operator<< (std::ostream & os, BroadcastHeader const  &);


/**
* \ingroup kitagawa
* \brief   Unicast (UNICASTTYPE) Message Format
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                        緊急車両現在地ID                        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    緊急車両のパケット送信時刻                  　　|
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                          送信元ID                           　  |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                        緊急車両目的地ID                        |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                          HopCount     　　　　　　　　　　　　　　　　　　　　　　　　|
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                            指示                           　   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  \endverbatim
*/
class UnicastHeader : public Header 
{
public:
  /// c-tor
  UnicastHeader (uint16_t e_start = 0, Time e_time = MilliSeconds(0), uint16_t source = 0, uint16_t e_dst = 0, uint16_t instruction = 0, uint16_t hopCount = 0);

  // Header serialization/deserialization
  static TypeId GetTypeId ();
  TypeId GetInstanceTypeId () const;
  uint32_t GetSerializedSize () const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void Print (std::ostream &os) const;

  // Fields
  void SetE_Start(uint16_t start)
  {
    e_start = start;
  }
  uint16_t GetE_Start() const
  {
    return e_start;
  }
  
  void SetTime(Time time)
  {
    e_time = time.GetMilliSeconds();
  }
  Time GetTime()
  {
    Time t(MilliSeconds(e_time));
    return t;
  }
   
  void SetSource(uint16_t mysource)
  {
    source = mysource;
  }
  uint16_t GetSource() const
  {
    return source;
  }

  void SetE_Dst(uint16_t destination)
  {
    e_dst = destination;
  }
  uint16_t GetE_Dst() const
  {
    return e_dst;
  }

  void SetInstruction(uint16_t inst)
  {
    instruction = inst;
  }
  uint16_t GetInstruction() const
  {
    return instruction;
  }
  
  void SetHopCount (uint16_t count) 
  { 
    m_hopCount = count; 
  }
  uint16_t GetHopCount () const 
  { 
    return m_hopCount; 
  }



private:
  uint16_t e_start;
  uint32_t e_time;
  uint16_t source;
  uint16_t e_dst;
  uint16_t instruction;
  uint16_t m_hopCount;
};

std::ostream & operator<< (std::ostream & os, UnicastHeader const  &);

/**
* \ingroup kitagawa
* \brief   Hello Message Format
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                           node Id                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                          Position                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  \endverbatim
*/

class HelloHeader : public Header
{
public:
  HelloHeader(uint16_t nodeId = 0, uint32_t position_X = 0, uint32_t position_Y = 0);

  static TypeId GetTypeId ();
  TypeId GetInstanceTypeId () const;
  uint32_t GetSerializedSize () const;
  void Serialize (Buffer::Iterator start) const;
  uint32_t Deserialize (Buffer::Iterator start);
  void Print (std::ostream &os) const;

  void SetNodeId(uint16_t nodeId)
    {
       m_nodeId = nodeId;
    }

  uint16_t GetNodeId()
    {
       return m_nodeId;
    }
  void SetPosition_X(uint32_t position_X)
    {
       m_position_X = position_X;
    }

  uint32_t GetPosition_X()
    {
       return m_position_X;
    }
  void SetPosition_Y(uint32_t position_Y)
    {
       m_position_Y = position_Y;
    }

  uint32_t GetPosition_Y()
    {
       return m_position_Y;
    }
     

private:
  uint16_t m_nodeId;
  uint32_t m_position_X;
  uint32_t m_position_Y;
};
std::ostream & operator<< (std::ostream & os, HelloHeader  const &);
  
}
}


#endif /* KITAGAWA_H */


