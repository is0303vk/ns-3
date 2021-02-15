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
#ifndef KITAGAWAROUTINGPROTOCOL_H
#define KITAGAWAROUTINGPROTOCOL_H


#include "kitagawa-packet.h"
#include "ns3/node.h"
#include "ns3/random-variable-stream.h"
#include "ns3/output-stream-wrapper.h"
#include "ns3/ipv4-routing-protocol.h"
#include "ns3/ipv4-interface.h"
#include "ns3/ipv4-l3-protocol.h"
#include <map>

namespace ns3 {
namespace kitagawa {
/**
 * \ingroup kitagawa
 *
 * \brief KITAGAWA routing protocol
 */
class KitagawaRoutingProtocol : public Ipv4RoutingProtocol
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  static const uint32_t KITAGAWA_PORT;

  /// constructor
  KitagawaRoutingProtocol ();
  virtual ~KitagawaRoutingProtocol ();
  virtual void DoDispose ();

  // Inherited from Ipv4RoutingProtocol
  Ptr<Ipv4Route> RouteOutput (Ptr<Packet> p, const Ipv4Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr);
  bool RouteInput (Ptr<const Packet> p, const Ipv4Header &header, Ptr<const NetDevice> idev,
                   UnicastForwardCallback ucb, MulticastForwardCallback mcb,
                   LocalDeliverCallback lcb, ErrorCallback ecb);
  virtual void NotifyInterfaceUp (uint32_t interface);
  virtual void NotifyInterfaceDown (uint32_t interface);
  virtual void NotifyAddAddress (uint32_t interface, Ipv4InterfaceAddress address);
  virtual void NotifyRemoveAddress (uint32_t interface, Ipv4InterfaceAddress address);
  virtual void SetIpv4 (Ptr<Ipv4> ipv4);
  virtual void PrintRoutingTable (Ptr<OutputStreamWrapper> stream, Time::Unit unit = Time::S) const;

  /*void SendBroadcast(void);
  void SendBroadcast2(void);
  void RecvKitagawa (Ptr<Socket> socket);
  void RecvBroadcast(Ptr<Packet> p, Ipv4Address receiver, Ipv4Address sender);
  void SendHello();
  void RecvHello(Ptr<Packet> p, Ipv4Address receiver, Ipv4Address sender);*/



  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this model.  Return the number of streams (possibly zero) that
   * have been assigned.
   *
   * \param stream first stream index to use
   * \return the number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);

protected:
  virtual void DoInitialize (void);
private:

  void SendBroadcast(void);
  void SendBroadcast2(uint16_t hop);
  void RecvKitagawa (Ptr<Socket> socket);
  void RecvBroadcast(Ptr<Packet> p, Ipv4Address receiver, Ipv4Address sender);
  void RecvBraodcast2(Ptr<Packet> p, Ipv4Address receiver, Ipv4Address sender);
  void RecvUnicast(Ptr<Packet> p, Ipv4Address receiver, Ipv4Address sender);
  void SendHello();
  void SendTo(Ptr<Socket> socket, Ptr<Packet> packet, Ipv4Address destination);     
  void RecvHello(Ptr<Packet> p, Ipv4Address receiver, Ipv4Address sender);
  void RecvPureFlooding(Ptr<Packet> p, Ipv4Address receiver, Ipv4Address sender);
  void SendPureFlooding(uint16_t hop);
  void CourseChange();
  void EmergencyCourseChange();
  void EmergencyCourseChange2();
  void NumberOfCarInEmergencyroute();
  void EmergencyDeleteTable();
  

  /// IP protocol
  Ptr<Ipv4> m_ipv4;
  /// Nodes IP address
  Ipv4Address m_mainAddress;
  /// Raw unicast socket per each IP interface, map socket -> iface address (IP + mask)
  std::map< Ptr<Socket>, Ipv4InterfaceAddress > m_socketAddresses;
  /// Raw subnet directed broadcast socket per each IP interface, map socket -> iface address (IP + mask)
  std::map< Ptr<Socket>, Ipv4InterfaceAddress > m_socketSubnetBroadcastAddresses;
 
  Ptr<NetDevice> m_lo;

  std::map<uint16_t,uint16_t> table{};
  std::map<uint16_t,uint16_t> table2{};
  std::map<uint16_t,uint16_t> table3{};
  std::map<uint16_t,uint16_t> table4{};
  std::map<uint16_t,uint16_t> connectedtable{};
  std::map<uint16_t,uint16_t> connectedtable2{};

  std::map<uint16_t,uint16_t> sendnumbertable{};
  std::map<uint16_t,uint16_t> recvnumbertable{};
  std::map<uint16_t,uint16_t> hoptable{};
  std::map<uint16_t,uint16_t> receive_node_table{};
  std::map<uint16_t,uint16_t> direction_table{};
 

 
  /// Provides uniform random variables.
  Ptr<UniformRandomVariable> m_uniformRandomVariable;

    /**
   * Find unicast socket with local interface address iface
   *
   * \param iface the interface
   * \returns the socket associated with the interface
   */
  Ptr<Socket> FindSocketWithInterfaceAddress (Ipv4InterfaceAddress iface) const;
  /**
   * Find subnet directed broadcast socket with local interface address iface
   *
   * \param iface the interface
   * \returns the socket associated with the interface
   */
  Ptr<Socket> FindSubnetBroadcastSocketWithInterfaceAddress (Ipv4InterfaceAddress iface) const;


};

} //namespace kitagawa
} //namespace ns3

#endif /* KITAGAWAROUTINGPROTOCOL_H */
