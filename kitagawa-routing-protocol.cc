/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 IITP RAS
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
 */


//有線併用型

#define NS_LOG_APPEND_CONTEXT                                   \
  if (m_ipv4) { std::clog << "[node " << m_ipv4->GetObject<Node> ()->GetId () << "] "; }

#define BUFFER_SIZE 1500

#include "kitagawa-routing-protocol.h"
#include "ns3/log.h"
#include "ns3/boolean.h"
#include "ns3/random-variable-stream.h"
#include "ns3/inet-socket-address.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/udp-socket-factory.h"
#include "ns3/udp-l4-protocol.h"
#include "ns3/udp-header.h"
#include "ns3/wifi-net-device.h"
#include "ns3/adhoc-wifi-mac.h"
#include "ns3/string.h"
#include "ns3/pointer.h"
#include "ns3/mobility-model.h"
#include "ns3/mobility-helper.h"
#include "ns3/constant-velocity-mobility-model.h"

#include <algorithm>
#include <limits>
#include "ns3/vector.h"


uint16_t e_dst_x = 1000;
uint16_t e_dst_y = 900;
uint16_t sequence; //Sequence number
uint16_t sum_TxCount = 0;
uint16_t RSU_TxCount = 0;
uint16_t Car_TxCount = 0;
uint16_t traffic_density = 0;
double number_of_data_in_emergency_route;  //接近通知パケット通信回数
double number_of_car_in_emergency_route;   //緊急車両ルート内の車両台数
double received_car_in_emergency_route;    //接近通知パケットを受信した車両台数
uint16_t arrived_destination_time = 0;                 

uint16_t count, count2;
uint16_t connectedcar[1000];
//変更--------------------------------
uint16_t emergencynode = 1000;
uint16_t RSUnode = 1001;
//uint16_t connectedcar = 250;
//------------------------------------

uint16_t RSUnode2 = RSUnode + 1;
uint16_t RSUnode3 = RSUnode + 2;
uint16_t RSUnode4 = RSUnode + 6;
uint16_t RSUnode5 = RSUnode + 10;
uint16_t RSUnode6 = RSUnode + 14;
uint16_t RSUnode7 = RSUnode + 15;
uint16_t RSUnode8 = RSUnode + 16;

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("KitagawaKitagawaRoutingProtocol");

namespace kitagawa {
NS_OBJECT_ENSURE_REGISTERED (KitagawaRoutingProtocol);

/// UDP Port for KITAGAWA control traffic
const uint32_t KitagawaRoutingProtocol::KITAGAWA_PORT = 654;



KitagawaRoutingProtocol::KitagawaRoutingProtocol ()
{
  
}

TypeId
KitagawaRoutingProtocol::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::kitagawa::KitagawaRoutingProtocol")
    .SetParent<Ipv4RoutingProtocol> ()
    .SetGroupName ("Kitagawa")
    .AddConstructor<KitagawaRoutingProtocol> ()
    .AddAttribute ("UniformRv",
                   "Access to the underlying UniformRandomVariable",
                   StringValue ("ns3::UniformRandomVariable"),
                   MakePointerAccessor (&KitagawaRoutingProtocol::m_uniformRandomVariable),
                   MakePointerChecker<UniformRandomVariable> ())
    /*.AddTraceSource("MacRx",
                    "A packet has been received by this device, has been passed up from the physical kayer"
                    "and is being forwarded up the local protocol stack. This is a non-promiscuous trace.",
                    MakeTraceSourceAccessor(&WifiMac::m_macRxTrace),
                    "ns3::Packet::TraceCallback")*/
  ;
  return tid;
}



KitagawaRoutingProtocol::~KitagawaRoutingProtocol ()
{
}

void
KitagawaRoutingProtocol::DoDispose ()
{
  m_ipv4 = 0;
  for (std::map<Ptr<Socket>, Ipv4InterfaceAddress>::iterator iter =
         m_socketAddresses.begin (); iter != m_socketAddresses.end (); iter++)
    {
      iter->first->Close ();
    }
  m_socketAddresses.clear ();

  for (std::map<Ptr<Socket>, Ipv4InterfaceAddress>::iterator iter2 =
         m_socketSubnetBroadcastAddresses.begin (); iter2 != m_socketSubnetBroadcastAddresses.end (); iter2++)
    {
      iter2->first->Close ();
    }
  m_socketSubnetBroadcastAddresses.clear ();
  
  Ipv4RoutingProtocol::DoDispose ();

  //printf("traffic density = %d\n",traffic_density);
  //printf("number_of_data_in_emergency_route = %f\n",number_of_data_in_emergency_route);
  //printf("number_of_car_in_emergency_route = %f\n",number_of_car_in_emergency_route);
  //printf("received_car_in_emergency_route = %f\n",received_car_in_emergency_route);
  
  if(count == 4){
    //printf("traffic density = %d\n",traffic_density);
    printf("number_of_data_in_emergency_route = %f\n",number_of_data_in_emergency_route);
    printf("number_of_car_in_emergency_route = %f\n",number_of_car_in_emergency_route);
    printf("received_car_in_emergency_route = %f\n",received_car_in_emergency_route);
    double Average_Communication_Coverage = received_car_in_emergency_route/number_of_car_in_emergency_route;
    double Average_Traffic_of_Data_in_Emergency_Route = number_of_data_in_emergency_route/number_of_car_in_emergency_route;
    printf("\nCommunication coverage = %f\n",Average_Communication_Coverage);
    printf("average traffic of data in Emergency Route = %f\n",Average_Traffic_of_Data_in_Emergency_Route);
    printf("arrived destination time = %d\n",arrived_destination_time);
    count++;
  }
}

void
KitagawaRoutingProtocol::PrintRoutingTable (Ptr<OutputStreamWrapper> stream, Time::Unit unit) const
{
  *stream->GetStream () << "Node: " << m_ipv4->GetObject<Node> ()->GetId ()
                        << "; Time: " << Now ().As (unit)
                        << ", Local time: " << GetObject<Node> ()->GetLocalTime ().As (unit)
                        << ", KITAGAWA Routing table" << std::endl;

  //Print routing table here.
  *stream->GetStream () << std::endl;
}

int64_t
KitagawaRoutingProtocol::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_uniformRandomVariable->SetStream (stream);
  return 1;
}



Ptr<Ipv4Route>
KitagawaRoutingProtocol::RouteOutput (Ptr<Packet> p, const Ipv4Header &header,
                              Ptr<NetDevice> oif, Socket::SocketErrno &sockerr)
{

  std::cout<<"Route Ouput Node: "<<m_ipv4->GetObject<Node> ()->GetId ()<<"\n";
  Ptr<Ipv4Route> route;

  if (!p)
    {
	  std::cout << "loopback occured! in routeoutput";
	  return route;// LoopbackRoute (header,oif);

	}

  if (m_socketAddresses.empty ())
    {
	  sockerr = Socket::ERROR_NOROUTETOHOST;
	  NS_LOG_LOGIC ("No kitagawa interfaces");
	  std::cout << "RouteOutput No kitagawa interfaces!!, packet drop\n";
                                                                                        
	  Ptr<Ipv4Route> route;
	  return route;
    }
  
  return route;
}



bool
KitagawaRoutingProtocol::RouteInput (Ptr<const Packet> p, const Ipv4Header &header,
                             Ptr<const NetDevice> idev, UnicastForwardCallback ucb,
                             MulticastForwardCallback mcb, LocalDeliverCallback lcb, ErrorCallback ecb)
{
  //std::cout<<"Route Input Node: "<<m_ipv4->GetObject<Node> ()->GetId ()<<"\n";
  return true;
}



void
KitagawaRoutingProtocol::SetIpv4 (Ptr<Ipv4> ipv4)
{
  NS_ASSERT (ipv4 != 0);
  NS_ASSERT (m_ipv4 == 0);
  m_ipv4 = ipv4;
}

void
KitagawaRoutingProtocol::NotifyInterfaceUp (uint32_t i)
{
  NS_LOG_FUNCTION (this << m_ipv4->GetAddress (i, 0).GetLocal ()
                        << " interface is up");
  Ptr<Ipv4L3Protocol> l3 = m_ipv4->GetObject<Ipv4L3Protocol> ();
  Ipv4InterfaceAddress iface = l3->GetAddress (i,0);
  if (iface.GetLocal () == Ipv4Address ("127.0.0.1"))
    {
      return;
    }
  // Create a socket to listen only on this interface
  Ptr<Socket> socket;

  socket = Socket::CreateSocket (GetObject<Node> (),UdpSocketFactory::GetTypeId ());
  NS_ASSERT (socket != 0);
  socket->SetRecvCallback (MakeCallback (&KitagawaRoutingProtocol::RecvKitagawa,this));
  socket->BindToNetDevice (l3->GetNetDevice (i));
  socket->Bind (InetSocketAddress (iface.GetLocal (), KITAGAWA_PORT));
  socket->SetAllowBroadcast (true);
  socket->SetIpRecvTtl (true);
  m_socketAddresses.insert (std::make_pair (socket,iface));


    // create also a subnet broadcast socket
  socket = Socket::CreateSocket (GetObject<Node> (),
                                 UdpSocketFactory::GetTypeId ());
  NS_ASSERT (socket != 0);
  socket->SetRecvCallback (MakeCallback (&KitagawaRoutingProtocol::RecvKitagawa, this));
  socket->BindToNetDevice (l3->GetNetDevice (i));
  socket->Bind (InetSocketAddress (iface.GetBroadcast (), KITAGAWA_PORT));
  socket->SetAllowBroadcast (true);
  socket->SetIpRecvTtl (true);
  m_socketSubnetBroadcastAddresses.insert (std::make_pair (socket, iface));


  if (m_mainAddress == Ipv4Address ())
    {
      m_mainAddress = iface.GetLocal ();
    }

  NS_ASSERT (m_mainAddress != Ipv4Address ());
}

void
KitagawaRoutingProtocol::NotifyInterfaceDown (uint32_t i)
{
}

void
KitagawaRoutingProtocol::NotifyAddAddress (uint32_t i, Ipv4InterfaceAddress address)
{ 
}

void
KitagawaRoutingProtocol::NotifyRemoveAddress (uint32_t i, Ipv4InterfaceAddress address)
{ 
}



void
KitagawaRoutingProtocol::DoInitialize (void)
{
//100*0.75=75 100*0.5=50 100*0.25=25
//500*0.75=375 500*0.5=250 500*0.75=125
//1000*0.25=750 1000*0.5=500 1000*0.75=250

//該当する車両IDのみが通信できる
if(count2==0){
  for(uint16_t j=0;j<250;j++){ //上記の数値を参考に変更
    uint16_t ccar = rand()%emergencynode; //車両台数
    connectedcar[j] = ccar;
  }
  count2++;
}
  
  SendHello();

  //if(m_ipv4->GetObject<Node> ()->GetId () == emergencynode)
    //Simulator::Schedule(Seconds(7),&KitagawaRoutingProtocol::EmergencyCourseChange,this); 
  //変更(通信なし)----------------------------------------------------------------------------
  Simulator::Schedule(Seconds(7),&KitagawaRoutingProtocol::NumberOfCarInEmergencyroute,this); 

 
/// 評価によって変更 -----------------------------------------------------------------
  uint16_t id = m_ipv4->GetObject<Node> ()->GetId ();
  if(id == RSUnode)   //消防署ノード
    {
      //Simulator::Schedule(Seconds(5), &KitagawaRoutingProtocol::SendBroadcast, this);
      Simulator::Schedule(Seconds(7), &KitagawaRoutingProtocol::SendBroadcast, this);
      //Simulator::Schedule(Seconds(9), &KitagawaRoutingProtocol::SendBroadcast, this);
    }
///---------------------------------------------------------------------------------------- 
  //--------------------------------------------------------------------------------------------
}

KitagawaRoutingProtocol::NumberOfCarInEmergencyroute()
{
  if(m_ipv4->GetObject<Node> ()->GetId ()<emergencynode){
    //変更 通信不搭載車の設定-------------------------------------------
    for(uint16_t z=0;z<250;z++){
      if(connectedcar[z] == m_ipv4->GetObject<Node> ()->GetId ()){
    //-----------------------------------------------------------------
   
    Ptr<MobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<MobilityModel>();
    Vector pos = mobility->GetPosition ();

    if(pos.x > 380 && pos.x < 420){
      number_of_car_in_emergency_route++;
    }
    if(pos.x <= 380){
      if(pos.y > 0 && pos.y < 20){
        number_of_car_in_emergency_route++;
      }
    }
    if(pos.x >= 420){
      if(pos.y >880 && pos.y < 920){
      number_of_car_in_emergency_route++;
      }
    }
    break;
    }
    }
  } 
}

void
KitagawaRoutingProtocol::RecvKitagawa (Ptr<Socket> socket)
{
  //printf("RecvKitagawa\n");
  Address sourceAddress;
  Ptr<Packet> packet = socket->RecvFrom (sourceAddress);
  InetSocketAddress inetSourceAddr = InetSocketAddress::ConvertFrom (sourceAddress);
  Ipv4Address sender = inetSourceAddr.GetIpv4 ();
  Ipv4Address receiver;

  if (m_socketAddresses.find (socket) != m_socketAddresses.end ())
    receiver = m_socketAddresses[socket].GetLocal ();
    

  TypeHeader tHeader (BROADCASTTYPE);
  packet->RemoveHeader (tHeader);

  switch(tHeader.Get()){
    case BROADCASTTYPE:
      RecvBroadcast(packet, receiver, sender);
      break;
    case BROADCASTTYPE2:
      RecvBroadcast(packet, receiver, sender);
      break;
    case BROADCASTTYPE3:
      RecvPureFlooding(packet, receiver, sender);
      break;
    case UNICASTTYPE:
      RecvUnicast(packet, receiver, sender);
      break;
    case HELLOTYPE:
      RecvHello(packet,receiver,sender);
      break;
    default:
      std::cout << "Unknown Type received in " << receiver << " from " << sender << "\n";
      break;

  }
}


void
KitagawaRoutingProtocol::RecvBroadcast (Ptr<Packet> p, Ipv4Address receiver, Ipv4Address sender)
{
  //printf("RecvBroadcast\n");
  //std::cout<<"In recvBroadcast(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n";

  //変更 通信不搭載車の設定-----------------------------------------------------------------------------------------
  for(uint16_t z=0;z<250;z++){
    if(connectedcar[z] == m_ipv4->GetObject<Node> ()->GetId ()|| m_ipv4->GetObject<Node> ()->GetId () > RSUnode){  //路側機ノードに合わせる
//---------------------------------------------------------------------------------------------------------------
  

  BroadcastHeader broadcastheader;
  p->RemoveHeader(broadcastheader);

  uint16_t sourceId = broadcastheader.GetSource();

  if(m_ipv4->GetObject<Node> ()->GetId () != emergencynode){ //緊急車両でないなら
    /*if(m_ipv4->GetObject<Node> ()->GetId () == 12){  //目的ノードが受信
      printf("arrived destination node, complete.\n");
      BroadcastHeader broadcastheader;
      p->RemoveHeader(broadcastheader);

      uint16_t lasthop = broadcastheader.GetHopCount();
      uint16_t lastsource = broadcastheader.GetSource();
      lasthop = lasthop + 1;

      printf("lasthop = %d\n",lasthop);
      printf("lastsource = %d\n",lastsource);
      
    } else {*/
      //BroadcastHeader broadcastheader;
      //p->RemoveHeader(broadcastheader);

      if(sequence == 2){
        sendnumbertable.clear();
      }
      if(sequence == 3){
        sendnumbertable.clear();
      }

      uint16_t mynodeId = m_ipv4->GetObject<Node> ()->GetId ();
      auto itr77 = sendnumbertable.find(mynodeId);

      if(itr77 != sendnumbertable.end()){
       //なにもしない
      } else {
        std::cout<<"In first recvBroadcast(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n";

      
        sendnumbertable.insert(std::make_pair(mynodeId, 0));

        uint16_t instruction = broadcastheader.GetInstruction();
        uint16_t hop = broadcastheader.GetHopCount();
        uint16_t source_type = broadcastheader.GetSource_Type();
        
        printf("sourceId = %d\n",sourceId);
        printf("instruction = %d\n",instruction);
        printf("type = %d\n" ,source_type);


        if(source_type == 0){ //送信元が消防署
          if(m_ipv4->GetObject<Node> ()->GetId () >=RSUnode ){
            SendWiredCommnication();
          }
        }

        if(source_type == 1){ //送信元がRSU
          if(m_ipv4->GetObject<Node> ()->GetId () >=RSUnode ){
            SendBroadcast3();
          }
        }

        if(source_type == 1 || source_type ==2){ //送信元がRSU or Car
          //printf("aaaaaaaaaaaaaaaaaa\n");
          if(m_ipv4->GetObject<Node> ()->GetId () == instruction){
            //printf("bbbbbbbbbbbbbbbbb\n");
            SendBroadcast2(hop+1);
                   
//緊急車両目的地到着時間測定のために一般車両を時空の彼方へ飛ばす 評価項目によって変更---------------------------------------------------------------
          }/*else{
            if(m_ipv4->GetObject<Node> ()->GetId () < emergencynode){
              Ptr<ConstantVelocityMobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<ConstantVelocityMobilityModel>();

              mobility->SetPosition(Vector(2000,2000,0));
            }
//--------------------------------------------------------------------------------------------------------------------------------------
          }*/ 
        }


        //一般車両モビリティ再制御
        if(m_ipv4->GetObject<Node> ()->GetId ()<emergencynode){
          Ptr<ConstantVelocityMobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<ConstantVelocityMobilityModel>();
          Vector pos = mobility->GetPosition();

          if(pos.x > 380 && pos.x < 420){
            //mobility->SetPosition(Vector(pos.x+10,pos.y,0));
            received_car_in_emergency_route++;
            //if(a==0)
              //mobility->SetVelocity(Vector(0,12,0));
            //if(a==1)
              //mobility->SetVelocity(Vector(0,-12,0));
          }
          if(pos.x <= 380){
            if(pos.y > 0 && pos.y < 20){
            //mobility->SetPosition(Vector(pos.x,pos.y+10,0));
              received_car_in_emergency_route++;
              //if(a==0)
                //mobility->SetVelocity(Vector(0,12,0));
              //if(a==1)
                //mobility->SetVelocity(Vector(0,-12,0));
            }      
          }
          if(pos.x >= 420){
            if(pos.y > 880 && pos.y <= 920){
            //mobility->SetPosition(Vector(pos.x,pos.y+10,0));
              received_car_in_emergency_route++;
              //if(a==0)
                //mobility->SetVelocity(Vector(0,12,0));
              //if(a==1)
                //mobility->SetVelocity(Vector(0,-12,0));
            }
          }
        }      
      } 
    //}
  }
  break;
  }
  }
}

void
KitagawaRoutingProtocol::SendWiredCommnication()
{
  //printf("SendWiredCommnication\n\n");
  if(m_ipv4->GetObject<Node> ()->GetId () == RSUnode2){
    std::cout<<"\nIn sendwirecommnication(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n\n";

    uint16_t RSU = 1;
       
    for (std::map<Ptr<Socket>, Ipv4InterfaceAddress>::const_iterator j = m_socketAddresses.begin (); j
        != m_socketAddresses.end (); ++j)
      {

        Ptr<Socket> socket = j->first;
        Ipv4InterfaceAddress iface = j->second;
        Ptr<Packet> packet = Create<Packet> ();

        BroadcastHeader broadcastheader;
        broadcastheader.SetSource(m_ipv4->GetObject<Node> ()->GetId ());
        broadcastheader.SetSource_Type(RSU);

        packet->AddHeader (broadcastheader);

///////////////////////////提案手法,比較手法でTYPEを決める///////////////////////////////////////

        TypeHeader tHeader (BROADCASTTYPE2);
        //TypeHeader tHeader (BROADCASTTYPE3);

///////////////////////////////////////////////////////////////////////////////////////////////

        packet->AddHeader (tHeader);
       
        // Send to all-hosts broadcast if on /32 addr, subnet-directed otherwise
        Ipv4Address destination;
        if (iface.GetMask () == Ipv4Mask::GetOnes ())
          {
            destination = Ipv4Address ("255.255.255.255");
          }
        else
          {
            destination = iface.GetBroadcast ();
          }
        socket->SendTo (packet, 0, InetSocketAddress (destination, KITAGAWA_PORT));         
       }
  }  
}


void
KitagawaRoutingProtocol::SendBroadcast (void) //緊急車両の無線Broadcast
{
  //uint16_t emergency = 0;
  uint16_t RSU = 1;
  //printf("nodeId == %d\n",m_ipv4->GetObject<Node> ()->GetId ());
     
  for (std::map<Ptr<Socket>, Ipv4InterfaceAddress>::const_iterator j = m_socketAddresses.begin (); j
       != m_socketAddresses.end (); ++j)
    {

      Ptr<Socket> socket = j->first;
      Ipv4InterfaceAddress iface = j->second;
      Ptr<Packet> packet = Create<Packet> ();

      BroadcastHeader broadcastheader;
      broadcastheader.SetE_Start(0);
      broadcastheader.SetTime(Now());
      broadcastheader.SetSource(m_ipv4->GetObject<Node> ()->GetId ());
      broadcastheader.SetE_Dst_X(e_dst_x);
      broadcastheader.SetE_Dst_Y(e_dst_y);
      //broadcastheader.SetInstruction(minNodeId);
      broadcastheader.SetHopCount(1);
      broadcastheader.SetSequence(sequence);
      broadcastheader.SetSource_Type(RSU);

      packet->AddHeader (broadcastheader);

///////////////////////////提案手法,比較手法でTYPEを決める///////////////////////////////////////

      TypeHeader tHeader (BROADCASTTYPE);
      //TypeHeader tHeader (BROADCASTTYPE3);

///////////////////////////////////////////////////////////////////////////////////////////////

      packet->AddHeader (tHeader);
       
      // Send to all-hosts broadcast if on /32 addr, subnet-directed otherwise
      Ipv4Address destination;
      if (iface.GetMask () == Ipv4Mask::GetOnes ())
        {
          destination = Ipv4Address ("255.255.255.255");
        }
      else
        {
          destination = iface.GetBroadcast ();
        }
      socket->SendTo (packet, 0, InetSocketAddress (destination, KITAGAWA_PORT));
      std::cout<<"\n\nbroadcast send start\n"; 
        
     }
             
        //printf("車両minNodeId = %d\n",minNodeId);
        //printf("minNodeId = %d\n",minNodeId);
        //printf("min = %d\n",min);
  
   //EmergencyCourseChange();
}


void
KitagawaRoutingProtocol::SendBroadcast2(uint16_t hop) //車両の無線Broadcast
{
  //printf("SendBroadcast2\n");

  //std::cout<<"\n\n\n\n\n\n\nIn send2(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n\n\n\n\n\n";

  if(m_ipv4->GetObject<Node>()->GetId() >=RSUnode){
    RSU_TxCount++;
  } else {
    Car_TxCount++;
  }
  //printf("sequence = %d\n",sequence);
  //printf("hop = %d\n",hop);
  if(hop<100){
    std::cout<<"\n\n\n\n\n\n\nIn send2(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n\n\n\n\n\n";   

    uint16_t mypos_x, mypos_y;
    uint32_t my_x, my_y;
    uint32_t mydistance;


    //車両用変数
    uint16_t Car = 2;
    uint16_t nodeId,nodeId2, minNodeId;
    uint32_t x[BUFFER_SIZE], y[BUFFER_SIZE], distance2[BUFFER_SIZE];
    uint32_t min = e_dst_x*e_dst_x + e_dst_y*e_dst_y;
    auto mainId =0;

        
///////////////テーブルの変数設定/////////////////////////
   
    //車両テーブル
    auto itr = table3.begin();
    auto itr2 = table4.begin();

    //std::cout << table3.size()<<"\n";


//////////////自分と目的ノードの距離//////////////////////
    
    Ptr<MobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<MobilityModel>();
    Vector mypos = mobility->GetPosition ();

    mypos_x = mypos.x;
    mypos_y = mypos.y;

    my_x = (e_dst_x-mypos_x)*(e_dst_x-mypos_x); 
    my_y = (e_dst_y-mypos_y)*(e_dst_y-mypos_y);

    //printf("e_dst_x = %d, e_dst_y = %d\n",e_dst_x,e_dst_y);
    //printf("mypos_x = %d, mypos_y = %d\n",mypos_x,mypos_y);

    mydistance = my_x + my_y;
    //printf("mydistance = %d\n",mydistance);

   
/////////////////Broadcast制御///////////////////////////////////////////////////////////       
    if(table3.size()>0){
      while(1){  //車両
        nodeId = itr->first;
        //printf("nodeId = %d\n",nodeId);
        uint16_t pos_x = itr->second;
        //printf("pos_x = %d\n",pos_x);        

        nodeId2 = itr2->first;
        //printf("nodeId2 = %d\n",nodeId2);
        uint16_t pos_y = itr2->second;
        //printf("pos_y = %d\n",pos_y);
        
        if(pos_x > 380 && pos_x <420){
          x[nodeId] = (e_dst_x-pos_x)*(e_dst_x-pos_x);
          //printf("nodeId = %d\n distance = %d\n",nodeId,x[nodeId]); 
          y[nodeId2] = (e_dst_y-pos_y)*(e_dst_y-pos_y);
          //printf("nodeId2 = %d\n distance = %d\n",nodeId2,y[nodeId2]); 
        }
        else if(pos_x <= 380){
          if(pos_y > 0  && pos_y < 20){
            x[nodeId] = (e_dst_x-pos_x)*(e_dst_x-pos_x); 
            y[nodeId2] = (e_dst_y-pos_y)*(e_dst_y-pos_y);
          }
        }
        else if(pos_x >= 420){
          if(pos_y > 880 && pos_y < 920){
            x[nodeId] = (e_dst_x-pos_x)*(e_dst_x-pos_x);
            y[nodeId2] = (e_dst_y-pos_y)*(e_dst_y-pos_y);
          }
        }
        else{
          x[nodeId] = 0;
          y[nodeId] = 0;
        }
   
        //printf("nodeId = %d\n distance = %d\n",nodeId,x[nodeId]); 
        //printf("nodeId2 = %d\n distance = %d\n",nodeId2,y[nodeId2]); 

        if(x[nodeId]==0 && y[nodeId2] ==0){
          distance2[mainId]=0;
        }else{ 
          distance2[mainId] = x[nodeId] + y[nodeId2]; //テーブル中の各ノードと目的までの距離
        }
        //printf("nodeId = %d\ndistance = %d\n",nodeId,distance2[mainId]);
        
        if(distance2[mainId] != 0){
          if(distance2[mainId] <= min ){  //目的ノードではない,かつ, 自分より目的地に近い
            if(mydistance <= distance2[mainId]){ //自分が目的ノードに近い
              min = mydistance;
              minNodeId = m_ipv4->GetObject<Node> ()->GetId ();
              //printf("min = %d\n",min);
              //printf("minNodeId = %d\n",minNodeId);
            } else {  //テーブル中の他のノードが目的ノードに近いとき
              min = distance2[mainId];
              minNodeId = nodeId;
              //printf("min = %d\n",min);
              //printf("minNodeId = %d\n",minNodeId);
            }
          }
        }
 
        itr++;
        itr2++;
        mainId++;
        //printf("minNodeId = %d\n",minNodeId);

        if(itr == table3.end()) break;    
      }
    }
    //printf("min = %d\n",min);
    //printf("minNodeId = %d\n",minNodeId);
    

    for (std::map<Ptr<Socket>, Ipv4InterfaceAddress>::const_iterator i =m_socketAddresses.begin (); i != m_socketAddresses.end (); i++)
    {
      Ptr<Socket> socket = i->first;
      Ipv4InterfaceAddress iface = i->second;
      BroadcastHeader broadcastheader;
      broadcastheader.SetSource(m_ipv4->GetObject<Node> ()->GetId ());
      broadcastheader.SetInstruction(minNodeId);
      broadcastheader.SetHopCount(hop);
      broadcastheader.SetSource_Type(Car);


      Ptr<Packet> packet = Create<Packet> ();
      packet->AddHeader (broadcastheader);

      TypeHeader tHeader (BROADCASTTYPE2);
      packet->AddHeader (tHeader);

      Ipv4Address destination;
      if (iface.GetMask () == Ipv4Mask::GetOnes ())
        {
          destination = Ipv4Address ("255.255.255.255");
        }
      else
        {
          destination = iface.GetBroadcast ();
        }

      socket->SendTo (packet, 0, InetSocketAddress (destination, KITAGAWA_PORT));
    }
  }
  sum_TxCount++;
  //printf("車両minNodeId = %d\n",minNodeId);
  //printf("minNodeId = %d\n",minNodeId);
  //printf("min = %d\n",min);
  //printf("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
  //printf("RSU_TxCount is %d\n",RSU_TxCount);
  //printf("Car_TxCount is %d\n",Car_TxCount);
  //printf("sum_TxCount is %d\n",sum_TxCount);

  /*uint16_t near_car = table3.size();
  if(near_car >= traffic_density){
    traffic_density = near_car;
    printf("traffic density = %d\n",traffic_density);
  }*/

  Ptr<MobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<MobilityModel>();
  Vector pos = mobility->GetPosition ();
  
  if(pos.x > 380 && pos.x <420){
    number_of_data_in_emergency_route++;
  }
  if(pos.x <= 380){
    if(pos.y > 0 && pos.y < 20){
      number_of_data_in_emergency_route++;
    }
  }
  if(pos.x >= 420){
    if(pos.y > 880 && pos.y < 920){
      number_of_data_in_emergency_route++;
    }
  }
  

  /*if(pos.x == 500){
    uint16_t near_car = table3.size();
    if(near_car > traffic_density){
      traffic_density = near_car;
      //printf("\n\n\n\n\n\n\n\n\ntraffic density = %d\n",traffic_density);
    }
  }
  if(pos.x < 500 && pos.y == 0){
    uint16_t near_car = table3.size();
    if(near_car > traffic_density){
      traffic_density = near_car;
      //printf("\n\n\n\n\n\n\n\ntraffic density = %d\n",traffic_density);
    }
  }
  if(pos.x > 500 && pos.y == 1000){
    uint16_t near_car = table3.size();
    if(near_car > traffic_density){
      traffic_density = near_car;
      //printf("\n\n\n\n\n\n\ntraffic density = %d\n",traffic_density);
    }
  }*/  
  //printf("traffic density = %d\n",traffic_density);

//緊急車両目的地到着時間測定のために一般車両を時空の彼方へ飛ばす 評価項目によって変更---------------------------------------------------------------
  //Ptr<ConstantVelocityMobilityModel> mobility2 = m_ipv4->GetObject<Node> ()->GetObject<ConstantVelocityMobilityModel>();

  //mobility2->SetPosition(Vector(2000,2000,0));
//--------------------------------------------------------------------------------------------------------------------------------
  
}

void
KitagawaRoutingProtocol::SendBroadcast3()  //路側機の無線Broadcast
{
  //printf("SendBroadcast3\n");
  //std::cout<<Simulator::Now().GetSeconds();
  //std::cout<<"\nIn sendbroadcast3(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n\n";

    if(m_ipv4->GetObject<Node> ()->GetId () == RSUnode2){
      Simulator::Schedule (Seconds(0), &KitagawaRoutingProtocol::SendBroadcast4, this);
    }else if(m_ipv4->GetObject<Node> ()->GetId () == RSUnode3){
      Simulator::Schedule (Seconds(0), &KitagawaRoutingProtocol::SendBroadcast4, this);
    }else if(m_ipv4->GetObject<Node> ()->GetId () == RSUnode4){
      Simulator::Schedule (Seconds(40), &KitagawaRoutingProtocol::SendBroadcast4, this);
    }else if(m_ipv4->GetObject<Node> ()->GetId () == RSUnode5){
      Simulator::Schedule (Seconds(60), &KitagawaRoutingProtocol::SendBroadcast4, this);
    }else if(m_ipv4->GetObject<Node> ()->GetId () == RSUnode6){
      Simulator::Schedule (Seconds(80), &KitagawaRoutingProtocol::SendBroadcast4, this);
    }else if(m_ipv4->GetObject<Node> ()->GetId () == RSUnode7){
      Simulator::Schedule (Seconds(100), &KitagawaRoutingProtocol::SendBroadcast4, this);
    }else if(m_ipv4->GetObject<Node> ()->GetId () == RSUnode8){
      Simulator::Schedule (Seconds(120), &KitagawaRoutingProtocol::SendBroadcast4, this);
    }

}

void
KitagawaRoutingProtocol::SendBroadcast4()  //路側機の無線Broadcast
{
  //printf("SendBroadcast4\n");
  std::cout<<Simulator::Now().GetSeconds();
  std::cout<<"\n\n\n\n\nIn sendbroadcast4(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n\n";

  uint16_t RSU = 1;

  uint16_t mypos_x, mypos_y;
  uint32_t my_x, my_y;
  uint32_t mydistance;

  uint16_t nodeId,nodeId2, minNodeId;
  uint32_t x[BUFFER_SIZE], y[BUFFER_SIZE], distance2[BUFFER_SIZE];
  uint32_t min = e_dst_x*e_dst_x + e_dst_y*e_dst_y;
  auto mainId =0;

        
///////////////テーブルの変数設定/////////////////////////
   
  //車両テーブル
  auto itr = table3.begin();
  auto itr2 = table4.begin();

  //////////////自分と目的ノードの距離//////////////////////
    
  Ptr<MobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<MobilityModel>();
  Vector mypos = mobility->GetPosition ();

  mypos_x = mypos.x;
  mypos_y = mypos.y;

  my_x = (e_dst_x-mypos_x)*(e_dst_x-mypos_x); 
  my_y = (e_dst_y-mypos_y)*(e_dst_y-mypos_y);

  //printf("e_dst_x = %d, e_dst_y = %d\n",e_dst_x,e_dst_y);
  //printf("mypos_x = %d, mypos_y = %d\n",mypos_x,mypos_y);

  mydistance = my_x + my_y;
  //printf("mydistance = %d\n",mydistance);

////////////////目的地までの最短ノード決定////////////////////////
  //std::cout << table3.size()<<"\n";
  if(table3.size()>0){
    while(1){  //車両
      nodeId = itr->first;
      //printf("nodeId = %d\n",nodeId);
      uint16_t pos_x = itr->second;
      //printf("pos_x = %d\n",pos_x);         

      nodeId2 = itr2->first;
      //printf("nodeId2 = %d\n",nodeId2);
      uint16_t pos_y = itr2->second;
      //printf("pos_y = %d\n",pos_y);

      if(pos_x > 380 && pos_x < 420){
        x[nodeId] = (e_dst_x-pos_x)*(e_dst_x-pos_x);
        //printf("nodeId = %d\n distance = %d\n",nodeId,x[nodeId]); 
        y[nodeId2] = (e_dst_y-pos_y)*(e_dst_y-pos_y);
        //printf("nodeId2 = %d\n distance = %d\n",nodeId2,y[nodeId2]); 
      }
      else if(pos_x <= 380){
        if(pos_y > 0 && pos_y < 20){
          x[nodeId] = (e_dst_x-pos_x)*(e_dst_x-pos_x); 
          y[nodeId2] = (e_dst_y-pos_y)*(e_dst_y-pos_y);
        }
      }
      else if(pos_x >= 420){
        if(pos_y > 880 && pos_y < 920){
          x[nodeId] = (e_dst_x-pos_x)*(e_dst_x-pos_x);
          y[nodeId2] = (e_dst_y-pos_y)*(e_dst_y-pos_y);
        }
      }
      else{
        x[nodeId]=0;
        y[nodeId2]=0;
      }

      if(x[nodeId]==0 && y[nodeId2]==0){
        distance2[mainId]=0;
      }else{
        distance2[mainId] = x[nodeId] + y[nodeId2]; //テーブル中の各ノードと目的までの距離
      }
      //printf("nodeId = %d\ndistance = %d\n",nodeId,distance2[mainId]);

      if(distance2[mainId] != 0){
        if(distance2[mainId] <= min ){  //目的ノードではない,かつ, 自分より目的地に近い
          if(mydistance <= distance2[mainId]){ //自分が目的ノードに近い
            min = mydistance;
            minNodeId = m_ipv4->GetObject<Node> ()->GetId ();
            //printf("min = %d\n",min);
            //printf("minNodeId = %d\n",minNodeId);
          } else {  //テーブル中の他のノードが目的ノードに近いとき
            min = distance2[mainId];
            minNodeId = nodeId;
            //printf("min = %d\n",min);
            //printf("minNodeId = %d\n",minNodeId);
          }
        }
      }
      //printf("min = %d\n",min);
      //printf("minNodeId = %d\n",minNodeId);
 
      itr++;
      itr2++;
      mainId++;

      if(itr == table3.end()) break;    
    }
  }
  //printf("min = %d\n",min);
  //printf("minNodeId = %d\n",minNodeId);

  //路車間通信開始
  for (std::map<Ptr<Socket>, Ipv4InterfaceAddress>::const_iterator i =m_socketAddresses.begin (); i != m_socketAddresses.end (); i++)
  {
    Ptr<Socket> socket = i->first;
    Ipv4InterfaceAddress iface = i->second;
    BroadcastHeader broadcastheader;
    broadcastheader.SetSource(m_ipv4->GetObject<Node> ()->GetId ());
    broadcastheader.SetSource_Type(RSU);
    broadcastheader.SetInstruction(minNodeId);

    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (broadcastheader);

    TypeHeader tHeader (BROADCASTTYPE2);
    packet->AddHeader (tHeader);

    

    Ipv4Address destination;
    if (iface.GetMask () == Ipv4Mask::GetOnes ())
      {
        destination = Ipv4Address ("255.255.255.255");
      }
    else
      {
        destination = iface.GetBroadcast ();
        
      }

    socket->SendTo (packet, 0, InetSocketAddress (destination, KITAGAWA_PORT));
  }

  number_of_data_in_emergency_route++;



//RSUの所持している隣接車両テーブルに緊急車両が存在する場合
  if(table3.count(emergencynode)){
    printf("RSUNode %d end of communicausion\n\n",m_ipv4->GetObject<Node> ()->GetId ());
  }else{
    Simulator::Schedule(Seconds(7), &KitagawaRoutingProtocol::SendBroadcast4, this);
  } 
   

  /*uint16_t near_car = table3.size();
  if(near_car >= traffic_density){
    traffic_density = near_car;
    printf("traffic density = %d\n",traffic_density);
  }*/
  
}

void
KitagawaRoutingProtocol::SendHello()
{
  
  //printf("SendHello\n");
  //std::cout<<"In sendhello(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n";
  //if(m_ipv4->GetObject<Node> ()->GetId ()==2)
    //std::cout<<"\n\n\n\n\n\nIn sendhello(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n\n\n\n\n\n";

  Ptr<MobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<MobilityModel>();
  Vector pos = mobility->GetPosition ();

  uint16_t simulator_time = Simulator::Now().GetSeconds();

  if(simulator_time <= 100){
  if(m_ipv4->GetObject<Node> ()->GetId () < RSUnode){ //一般車両なら
    //if(m_ipv4->GetObject<Node> ()->GetId () == 30){
      //std::cout<<Simulator::Now().GetSeconds();
      //std::cout<<"In sendhello(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n";
    //}
    for (std::map<Ptr<Socket>, Ipv4InterfaceAddress>::const_iterator j = m_socketAddresses.begin (); j != m_socketAddresses.end (); ++j)
      {
         Ptr<Socket> socket = j->first;
         Ipv4InterfaceAddress iface = j->second;
         HelloHeader helloHeader;
         Ptr<Packet> packet = Create<Packet> ();
         helloHeader.SetNodeId(m_ipv4->GetObject<Node> ()->GetId ());
         helloHeader.SetPosition_X(pos.x);
         helloHeader.SetPosition_Y(pos.y);
         packet->AddHeader(helloHeader);

         TypeHeader tHeader (HELLOTYPE);
         packet->AddHeader (tHeader);

         Ipv4Address destination;

         if (iface.GetMask () == Ipv4Mask::GetOnes ())
           {
            destination = Ipv4Address ("255.255.255.255");
           }
         else
           { 
             destination = iface.GetBroadcast ();
           }
         Time jitter = Time (MilliSeconds (m_uniformRandomVariable->GetInteger (1000,3000)));
         Simulator::Schedule (jitter, &KitagawaRoutingProtocol::SendTo, this , socket, packet, destination);
         Simulator::Schedule (jitter, &KitagawaRoutingProtocol::SendHello, this);
         //socket->SendTo (packet, 0, InetSocketAddress (destination, KITAGAWA_PORT));
      }
  }
  }else if(simulator_time > 100 && simulator_time < 200){
  if(m_ipv4->GetObject<Node> ()->GetId () < RSUnode){  //一般車両なら
  if(pos.y > 100){

    for (std::map<Ptr<Socket>, Ipv4InterfaceAddress>::const_iterator j = m_socketAddresses.begin (); j != m_socketAddresses.end (); ++j)
      {
         Ptr<Socket> socket = j->first;
         Ipv4InterfaceAddress iface = j->second;
         HelloHeader helloHeader;
         Ptr<Packet> packet = Create<Packet> ();
         helloHeader.SetNodeId(m_ipv4->GetObject<Node> ()->GetId ());
         helloHeader.SetPosition_X(pos.x);
         helloHeader.SetPosition_Y(pos.y);
         packet->AddHeader(helloHeader);

         TypeHeader tHeader (HELLOTYPE);
         packet->AddHeader (tHeader);

         Ipv4Address destination;

         if (iface.GetMask () == Ipv4Mask::GetOnes ())
           {
            destination = Ipv4Address ("255.255.255.255");
           }
         else
           { 
             destination = iface.GetBroadcast ();
           }
         Time jitter = Time (MilliSeconds (m_uniformRandomVariable->GetInteger (1000,3000)));
         Simulator::Schedule (jitter, &KitagawaRoutingProtocol::SendTo, this , socket, packet, destination);
         Simulator::Schedule (jitter, &KitagawaRoutingProtocol::SendHello, this);
      }
  }
  }
  }else{
   if(m_ipv4->GetObject<Node> ()->GetId () < RSUnode){ //一般車両なら
   if(pos.y > 600){

    for (std::map<Ptr<Socket>, Ipv4InterfaceAddress>::const_iterator j = m_socketAddresses.begin (); j != m_socketAddresses.end (); ++j)
      {
         Ptr<Socket> socket = j->first;
         Ipv4InterfaceAddress iface = j->second;
         HelloHeader helloHeader;
         Ptr<Packet> packet = Create<Packet> ();
         helloHeader.SetNodeId(m_ipv4->GetObject<Node> ()->GetId ());
         helloHeader.SetPosition_X(pos.x);
         helloHeader.SetPosition_Y(pos.y);
         packet->AddHeader(helloHeader);

         TypeHeader tHeader (HELLOTYPE);
         packet->AddHeader (tHeader);


         Ipv4Address destination;

         if (iface.GetMask () == Ipv4Mask::GetOnes ())
           {
            destination = Ipv4Address ("255.255.255.255");
           }
         else
           { 
             destination = iface.GetBroadcast ();
           }
         Time jitter = Time (MilliSeconds (m_uniformRandomVariable->GetInteger (1000,3000)));
         Simulator::Schedule (jitter, &KitagawaRoutingProtocol::SendTo, this , socket, packet, destination);
         Simulator::Schedule (jitter, &KitagawaRoutingProtocol::SendHello, this);
      }
  }
  }
  }
  
     


  /*if(pos.x == 500){
    uint16_t near_car = table3.size();
    if(near_car > traffic_density){
      traffic_density = near_car;
      //printf("\n\n\n\n\n\n\n\n\ntraffic density = %d\n",traffic_density);
    }
  }
  if(pos.x < 500 && pos.y == 0){
    uint16_t near_car = table3.size();
    if(near_car > traffic_density){
      traffic_density = near_car;
      //printf("\n\n\n\n\n\n\n\ntraffic density = %d\n",traffic_density);
    }
  }
  if(pos.x > 500 && pos.y == 1000){
    uint16_t near_car = table3.size();
    if(near_car > traffic_density){
      traffic_density = near_car;
      //printf("\n\n\n\n\n\n\ntraffic density = %d\n",traffic_density);
    }
  }*/  
  //printf("traffic density = %d\n",traffic_density);

  //緊急車両経路制御    
  if(m_ipv4->GetObject<Node> ()->GetId () == emergencynode){
    Ptr<ConstantVelocityMobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<ConstantVelocityMobilityModel>();
    Vector pos = mobility->GetPosition ();
    uint16_t pos_x = pos.x;
    uint16_t pos_y = pos.y;

    std::cout<<Simulator::Now().GetSeconds();
    printf("緊急車両の位置 %d %d\n",pos_x,pos_y);
            
    if(pos.x >= 400){
      if(count == 0){
        mobility->SetPosition(Vector(400,0,0));
        mobility->SetVelocity(Vector(0,12,0));
        printf("第一曲がり角\n");
        count++;
      }
    }
    if(pos.y >= 900){
      if(count == 1){
        mobility->SetPosition(Vector(400,900,0));
        mobility->SetVelocity(Vector(12,0,0));
        count++;
        printf("第二曲がり角\n");
      }      
    }
    //緊急車両の目的地到着時間計測
    if(pos.x >= 950){
      if(count == 2){
        arrived_destination_time = Simulator::Now().GetSeconds();
        //printf("arrived destination time = %d\n",arrived_destination_time);
        count++;
      }
    }
    if(pos.x >=1000){
      if(count == 3){
        mobility->SetPosition(Vector(1000,900,0));
        mobility->SetVelocity(Vector(0,0,0));
        printf("\n\n\narrived desitinetion!!\n\n\n\n");
        count++;
      }
    }
       
  } 
  
        
}

void 
KitagawaRoutingProtocol::SendTo(Ptr<Socket> socket, Ptr<Packet> packet, Ipv4Address destination)
{
  //printf("SendTo\n");
  socket->SendTo (packet, 0, InetSocketAddress (destination, KITAGAWA_PORT));
  
}

void
KitagawaRoutingProtocol::RecvHello(Ptr<Packet> packet, Ipv4Address receiver, Ipv4Address sender)
{
  //printf("RecvHello\n");
  //std::cout<<"In recvhello(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n";
  HelloHeader helloHeader;
  packet->RemoveHeader(helloHeader);
  uint16_t nodeId = helloHeader.GetNodeId();
  uint16_t pos_x = helloHeader.GetPosition_X();
  uint16_t pos_y = helloHeader.GetPosition_Y();
  //printf("nodeId=%d\n",nodeId);

  //緊急車両の遅延設定
  if(m_ipv4->GetObject<Node> ()->GetId () == emergencynode){ //緊急車両なら

    //変更(通信なし)------------------------------
    if(nodeId > emergencynode){ //路側機ノード
    //------------------------------------------
      auto itr = table.find(nodeId);
      if(itr != table.end()){
        table.erase(itr);
        table.insert(std::make_pair(nodeId, pos_x));
      }else{
        table.insert(std::make_pair(nodeId, pos_x));
      }

      auto itr2 = table2.find(nodeId);
      if(itr2 != table2.end()){
        table2.erase(itr2);
        table2.insert(std::make_pair(nodeId, pos_y));
      }else {
        table2.insert(std::make_pair(nodeId, pos_y));
      }
    }else{//車両ノード
      auto itr3 = table3.find(nodeId);
      if(itr3 != table3.end()){
        table3.erase(itr3);
        table3.insert(std::make_pair(nodeId, pos_x));
      }else{
        table3.insert(std::make_pair(nodeId, pos_x));
      }
      auto itr4 = table4.find(nodeId);
      if(itr4 != table4.end()){
        table4.erase(itr4);
        table4.insert(std::make_pair(nodeId, pos_y));
      }else{
        table4.insert(std::make_pair(nodeId, pos_y));
      }
    }

    EmergencyCourseChange();

  }else{
    //変更(通信なし)-------------------------
    if(nodeId > emergencynode){ //路側機ノード
    //------------------------------------
      auto itr = table.find(nodeId);
      if(itr != table.end()){
        table.erase(itr);
        table.insert(std::make_pair(nodeId, pos_x));
      }else{
        table.insert(std::make_pair(nodeId, pos_x));
      }

      auto itr2 = table2.find(nodeId);
      if(itr2 != table2.end()){
        table2.erase(itr2);
        table2.insert(std::make_pair(nodeId, pos_y));
      }else{
        table2.insert(std::make_pair(nodeId, pos_y));
      }
    }else{//車両ノード
    //変更 通信不搭載車の設定-----------------------------------------------------------
      for(uint16_t z=0;z<250;z++){
        if(connectedcar[z] == nodeId || nodeId == emergencynode){ //緊急車両ノード
    //---------------------------------------------------------------------------------
          auto itr3 = table3.find(nodeId);
          if(itr3 != table3.end()){
            table3.erase(itr3);
            table3.insert(std::make_pair(nodeId, pos_x));
          }else{
            table3.insert(std::make_pair(nodeId, pos_x));
          }
          auto itr4 = table4.find(nodeId);
          if(itr4 != table4.end()){
            table4.erase(itr4);
            table4.insert(std::make_pair(nodeId, pos_y));
          }else{
            table4.insert(std::make_pair(nodeId, pos_y));
          }
        }    
      }
    }
  }                
}

void
KitagawaRoutingProtocol::EmergencyCourseChange()
{
  uint32_t distance;
  uint32_t most_near_distance = 9999;

  
  
  //車両テーブル
  auto itr = table3.begin();
  auto itr2 = table4.begin();

  //自分の位置情報
  Ptr<ConstantVelocityMobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<ConstantVelocityMobilityModel>();
  Vector mypos = mobility->GetPosition ();

  uint32_t mypos_x = mypos.x;
  uint32_t mypos_y = mypos.y;
  //printf("mypos_x= %d\nmypos_y=%d\n",mypos_x,mypos_y);

  if(table3.size()>0){
    while(1){ 
      //nodeId = itr->first;
      //printf("nodeId = %d\n",nodeId);
      uint16_t pos_x = itr->second;
      //printf("pos_x = %d\n",pos_x);

      //nodeId2 = itr2->first;
      //printf("nodeId2 = %d\n",nodeId2);
      uint16_t pos_y = itr2->second;
      //printf("pos_y = %d\n",pos_y);
     
      distance = (mypos_x-pos_x)*(mypos_x-pos_x)+(mypos_y-pos_y)*(mypos_y-pos_y);
      //printf("distance = %d\n",distance);
      if(distance>2500){
        table3.erase(itr);
        table4.erase(itr2);
      }

      if(most_near_distance > distance){
        most_near_distance = distance;
        
      }

      itr++;
      itr2++;
         
      if(itr == table3.end()) break;    
    }
  }

  //printf("most_near_distance = %d\n",most_near_distance);

  if(most_near_distance<=2500 && most_near_distance>1225){
    mobility->SetVelocity(Vector(0,0,0));
    
    Simulator::Schedule (Seconds(4), &KitagawaRoutingProtocol::EmergencyCourseChange2, this);
  }

  if(most_near_distance<=1225 && most_near_distance>400){
    mobility->SetVelocity(Vector(0,0,0));
    
    Simulator::Schedule (Seconds(3), &KitagawaRoutingProtocol::EmergencyCourseChange2, this);
  }

  if(most_near_distance<=400 && most_near_distance>0){
    mobility->SetVelocity(Vector(0,0,0));
    
    Simulator::Schedule (Seconds(2), &KitagawaRoutingProtocol::EmergencyCourseChange2, this);
  }

  //table3.clear();
  //table4.clear();
    if(count2==1){
    Simulator::Schedule (Seconds(5), &KitagawaRoutingProtocol::EmergencyDeleteTable, this);
    count2++;
  }  

  //Simulator::Schedule (Seconds(3), &KitagawaRoutingProtocol::EmergencyCourseChange, this);
     
}


void
KitagawaRoutingProtocol::EmergencyCourseChange2()
{
  Ptr<ConstantVelocityMobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<ConstantVelocityMobilityModel>();
  Vector mypos = mobility->GetPosition ();

  if(mypos.y == 0 || mypos.y==900){
    mobility->SetVelocity(Vector(12,0,0));
  }else {
    mobility->SetVelocity(Vector(0,12,0));
  }
}

void
KitagawaRoutingProtocol::EmergencyDeleteTable()
{
  table3.clear();
  table4.clear();

  Simulator::Schedule (Seconds(3), &KitagawaRoutingProtocol::EmergencyDeleteTable, this);   
}


void
KitagawaRoutingProtocol::RecvUnicast(Ptr<Packet> packet, Ipv4Address receiver, Ipv4Address sender)
{
  //printf("RecvUnicast\n");
}

void
KitagawaRoutingProtocol::SendPureFlooding(uint16_t hop)
{
  //printf("SendPureFlooding\n");
}

void
KitagawaRoutingProtocol::RecvPureFlooding(Ptr<Packet> packet, Ipv4Address receiver, Ipv4Address sender)
{
  //printf("RecvPureFlooding\n");
}

} //namespace kitagawa
} //namespace ns3
