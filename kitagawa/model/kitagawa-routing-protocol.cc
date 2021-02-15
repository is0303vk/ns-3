
#define NS_LOG_APPEND_CONTEXT                                   \
  if (m_ipv4) { std::clog << "[node " << m_ipv4->GetObject<Node> ()->GetId () << "] "; }

#define BUFFER_SIZE 1200

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
#include "ns3/packet-loss-counter.h"

#include <algorithm>
#include <limits>
#include "ns3/vector.h"

uint16_t e_dst_x;
uint16_t e_dst_y;
uint8_t sequence; //Sequence number
uint8_t traffic_density;
double number_of_data_in_emergency_route;
double number_of_car_in_emergency_route;
double received_car_in_emergency_route;
uint16_t arrived_destination_time;

double receive_node_size;
uint8_t number_of_send;
uint8_t count;
uint16_t count2;
uint16_t connectedcar[1000];

//変更-------------------------------------
uint16_t emergencycar = 500;
uint16_t RSU = emergencycar+1;
uint16_t destination = emergencycar+100;
//-----------------------------------------


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
  
  
  if(count == 21){
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
  /**stream->GetStream () << "Node: " << m_ipv4->GetObject<Node> ()->GetId ()
                        << "; Time: " << Now ().As (unit)
                        << ", Local time: " << GetObject<Node> ()->GetLocalTime ().As (unit)
                        << ", KITAGAWA Routing table" << std::endl;

  //Print routing table here.
  *stream->GetStream () << std::endl;*/
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
/*
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
  */
  return route;
}



bool
KitagawaRoutingProtocol::RouteInput (Ptr<const Packet> p, const Ipv4Header &header,
                             Ptr<const NetDevice> idev, UnicastForwardCallback ucb,
                             MulticastForwardCallback mcb, LocalDeliverCallback lcb, ErrorCallback ecb)
{
  std::cout<<"Route Input Node: "<<m_ipv4->GetObject<Node> ()->GetId ()<<"\n";
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
  for(uint16_t j=0;j<125;j++){ //上記の数値を参考に変更
    uint16_t ccar = rand()%emergencycar; //車両台数
    connectedcar[j] = ccar;
  }
  count2++;
}
  SendHello();
  Simulator::Schedule(Seconds(7),&KitagawaRoutingProtocol::NumberOfCarInEmergencyroute,this); 
  uint16_t id = m_ipv4->GetObject<Node> ()->GetId ();

  if(id == emergencycar){   //緊急車両ノードなら
    Simulator::Schedule(Seconds(7), &KitagawaRoutingProtocol::SendBroadcast, this);
    //Simulator::Schedule(Seconds(7), &KitagawaRoutingProtocol::SendBroadcast, this);
    //Simulator::Schedule(Seconds(9), &KitagawaRoutingProtocol::SendBroadcast, this);
  }
}

void
KitagawaRoutingProtocol::NumberOfCarInEmergencyroute()
{
  if(m_ipv4->GetObject<Node> ()->GetId ()<emergencycar){ //一般車両なら
//変更 通信不搭載車の設定----------------------------------------------
    for(uint16_t z=0;z<125;z++){
      if(connectedcar[z] == m_ipv4->GetObject<Node> ()->GetId ()){
//-------------------------------------------------------------------

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
//変更 通信不搭載車の設定-----------------------------------------------------------------------------------------
  for(uint16_t z=0;z<125;z++){
    if(connectedcar[z] == m_ipv4->GetObject<Node> ()->GetId () || m_ipv4->GetObject<Node> ()->GetId () >= RSU){  //路側機ノードに合わせる
//---------------------------------------------------------------------------------------------------------------
  std::cout<<"In recvBroadcast(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n";

  if(m_ipv4->GetObject<Node> ()->GetId () != emergencycar){ //緊急車両じゃないなら
    if(m_ipv4->GetObject<Node> ()->GetId () == destination){  //目的ノードなら
      printf("arrived destination node, complete.\n");
      BroadcastHeader broadcastheader;
      p->RemoveHeader(broadcastheader);

      uint16_t lasthop = broadcastheader.GetHopCount();
      uint16_t lastsource = broadcastheader.GetSource();
      lasthop = lasthop + 1;

      //printf("lasthop = %d\n",lasthop);
      printf("lastsource = %d\n",lastsource);
      
    }
    if(m_ipv4->GetObject<Node> ()->GetId () != destination){ //目的地じゃないなら
      BroadcastHeader broadcastheader;
      p->RemoveHeader(broadcastheader);

      if(sequence == 2){
        recvnumbertable.clear();
      }
      if(sequence == 3){
        recvnumbertable.clear();
      }

      uint16_t mynodeId = m_ipv4->GetObject<Node> ()->GetId ();
      auto itr77 = recvnumbertable.find(mynodeId);

      if(itr77 != recvnumbertable.end()){
         if(m_ipv4->GetObject<Node> ()->GetId ()<emergencycar){ //一般車両なら
           Ptr<ConstantVelocityMobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<ConstantVelocityMobilityModel>();
         //緊急車両目的到着時間測定 変更--------------------------------
           mobility->SetPosition(Vector(2000,2000,0));
         //------------------------------------------------------------
         }
      } else {
        recvnumbertable.insert(std::make_pair(mynodeId, 0));
        
        if(m_ipv4->GetObject<Node> ()->GetId ()<emergencycar){ //一般車両なら
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
       

        uint16_t hop = broadcastheader.GetHopCount();
        uint8_t instruction = broadcastheader.GetInstruction();
        uint16_t sourceId = broadcastheader.GetSource();


        uint16_t mypos_x, mypos_y;
        uint32_t my_x, my_y;
        uint32_t mydistance=0;

        //路側機用変数  
        uint16_t nodeId,nodeId2, minNodeId = 0;
        uint16_t nId, nId2, nId3, mId= 0;
        uint32_t x[BUFFER_SIZE], y[BUFFER_SIZE], distance[BUFFER_SIZE];
        uint32_t x2[BUFFER_SIZE], y2[BUFFER_SIZE], source_other_distance[BUFFER_SIZE];
        uint32_t x3, y3, source_destination_distance= 0;
        uint32_t min = e_dst_x*e_dst_x + e_dst_y*e_dst_y;
        auto mainId = 0;

        //車両用変数
        uint16_t nodeId3,nodeId4, minNodeId3 = 0;
        uint16_t nId4, nId5, nId6, mId2= 0;
        uint32_t x4[BUFFER_SIZE], y4[BUFFER_SIZE], distance2[BUFFER_SIZE];
        uint32_t x5[BUFFER_SIZE], y5[BUFFER_SIZE], source_other_distance2[BUFFER_SIZE];
        uint32_t min2 = e_dst_x*e_dst_x + e_dst_y*e_dst_y;
        auto mainId2 = 0;

        
///////////////テーブルの変数設定/////////////////////////
        printf("sourceId = %d\n",sourceId);

        //路側機テーブル
        auto itr = table.begin();
        auto itr2 = table2.begin();
        auto itr3 = table.begin();
        auto itr4 = table2.begin();
        auto itr5 = table.begin();
        auto itr6 = table2.begin();
   
        //車両テーブル
        auto itr7 = table3.begin();
        auto itr8 = table4.begin();
        auto itr9 = table3.begin();
        auto itr10 = table4.begin();
        auto itr11 = table3.begin();
        auto itr12 = table4.begin();
      
        //std::cout << table.size()<<"\n";
        //std::cout << table2.size()<<"\n";
        //std::cout << table3.size()<<"\n";
        //std::cout << table4.size()<<"\n";

//////////////自分と目的ノードの距離//////////////////////
    
        Ptr<MobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<MobilityModel>();
        Vector mypos = mobility->GetPosition ();

        mypos_x = mypos.x;
        mypos_y = mypos.y;

        my_x = (e_dst_x-mypos_x)*(e_dst_x-mypos_x); 
        my_y = (e_dst_y-mypos_y)*(e_dst_y-mypos_y);

        mydistance = my_x + my_y;
        //printf("e_dst_x = %d\ne_dst_y = %d\n",e_dst_x,e_dst_y);
        //printf("mypos_x = %d\nmypos_y = %d\n",mypos_x,mypos_y);
        //printf("mydistance = %d\n",mydistance);

//////////////送信元ノードと目的ノードとの距離//////////////////////////////////////

        uint16_t source_pos_x;
        uint16_t source_pos_y;

        if(table.size()>0){
          while(1){  //路側機
            nId3 = itr5->first;
            //printf("nId3 = %d\n",nId3);
        
            uint16_t pos_x = itr5->second;
            uint16_t pos_y = itr6->second;

            if(sourceId == nId3){
              source_pos_x = pos_x;  //sourceのX
              source_pos_y = pos_y;  //sourceのY
              //printf("source_pos_x = %d\n",source_pos_x);
              //printf("source_pos_y = %d\n",source_pos_y);
            } 

            itr5++;
            itr6++;

            if(itr5 == table.end()) break;  
          }
        }

        if(table3.size()>0){
          while(1){  //車両
            nId6 = itr11->first;
            //printf("nId6 = %d\n",nId6);
        
            uint16_t pos_x = itr11->second;
            uint16_t pos_y = itr12->second;

            if(sourceId == nId6){
              source_pos_x = pos_x;  //sourceのX
              source_pos_y = pos_y;  //sourceのY
              //printf("source_pos_x = %d\n",source_pos_x);
              //printf("source_pos_y = %d\n",source_pos_y);
            }

            itr11++;
            itr12++;

            if(itr11 == table3.end()) break;  
          }
        }


        x3 = (e_dst_x-source_pos_x)*(e_dst_x-source_pos_x);
        y3 = (e_dst_y-source_pos_y)*(e_dst_y-source_pos_y);
        source_destination_distance = x3 + y3; //送信ノードと目的ノードまでの距離
        //printf("source_destination_distance = %d\n",source_destination_distance);

////////////送信ノードとテーブルノードの距離(自分以外ノード)////////////////////////////
        if(table.size()>0){
          while(1){//路側機
          nId = itr3->first;
          //printf("nId = %d\n",nId);
          uint16_t pos_x = itr3->second;
          //printf("pos_x = %d\n",pos_x);

          nId2 = itr4->first;
          uint16_t pos_y = itr4->second;
          //printf("pos_y = %d\n",pos_y);

          x2[nId] = (source_pos_x-pos_x)*(source_pos_x-pos_x);
          y2[nId2] = (source_pos_y-pos_y)*(source_pos_y-pos_y);
          //printf("x2[nId] = %d\n",x2[nId]);
          //printf("y2[nId2] = %d\n",y2[nId2]);
          source_other_distance[mId] = x2[nId] + y2[nId2]; //送信ノードと他ノードの距離
          //printf("source_other_distance = %d\n",source_other_distance[mId]);

          itr3++;
          itr4++;
          mId++;
          if(itr3 == table.end()) break;
          }
        }

        if(table3.size()>0){
          while(1){//車両
            nId4 = itr9->first;
            //printf("nId4 = %d\n",nId4);
            uint16_t pos_x = itr9->second;
            //printf("pos_x = %d\n",pos_x);

            nId5 = itr10->first;
            uint16_t pos_y = itr10->second;
            //printf("pos_y = %d\n",pos_y);

            x5[nId4] = (source_pos_x-pos_x)*(source_pos_x-pos_x);
            y5[nId5] = (source_pos_y-pos_y)*(source_pos_y-pos_y);
            //printf("x5[nId4] = %d\n",x5[nId4]);
            //printf("y5[nId5] = %d\n",y5[nId5]);
            source_other_distance2[mId2] = x5[nId4] + y5[nId5]; //送信ノードと他ノードの距離
            //printf("source_other_distance2 = %d\n",source_other_distance2[mId2]);        

            itr9++;
            itr10++;
            mId2++;
            //printf("a\n");
            if(itr9 == table3.end()) break;
          }
        }

////////////////////大型車両強制送信///////////////////////////////////////////////////////////
        /*if(m_ipv4->GetObject<Node> ()->GetId ()==20 || m_ipv4->GetObject<Node> ()->GetId ()==30 || m_ipv4->GetObject<Node> ()->GetId ()==40 || m_ipv4->GetObject<Node> ()->GetId ()==50){
          if(mydistance<source_destination_distance){
            SendBroadcast2(hop+1);
          }
        } else {*/   
/////////////////Broadcast制御///////////////////////////////////////////////////////////        
          if(table.size()>0){
            while(1){  //路側機
              nodeId = itr->first;
              //printf("nodeId = %d\n",nodeId);
              uint16_t pos_x = itr->second;
              //printf("pos_x = %d\n",pos_x);
              x[nodeId] = (e_dst_x-pos_x)*(e_dst_x-pos_x);
              //printf("nodeId = %d\n distance = %d\n",nodeId,x[nodeId]);

              nodeId2 = itr2->first;
              //printf("nodeId2 = %d\n",nodeId2);
              uint16_t pos_y = itr2->second;
              //printf("pos_y = %d\n",pos_y);
              y[nodeId2] = (e_dst_y-pos_y)*(e_dst_y-pos_y);
        
              distance[mainId] = x[nodeId] + y[nodeId2]; //テーブル中の各ノードと目的までの距離
              //printf("nodeId = %d\n distance = %d\n",nodeId,distance[mainId]);
        

              if(distance[mainId] <= min ){  //目的ノードではない
                if(mydistance < distance[mainId]){ //自分が目的ノードに近い
                  min = mydistance;
                  minNodeId = m_ipv4->GetObject<Node> ()->GetId ();
                  //printf("min = %d\n",min);
                  //printf("minNodeId = %d\n",minNodeId);
                } else if(mydistance == distance[mainId]) { //同距離のノードが存在している場合
                  //minNodeId = m_ipv4->GetObject<Node> ()->GetId ();
                } else {  //テーブル中の他のノードが目的ノードに近いとき
                  if(source_other_distance[mainId]<10000){ //通信範囲内なら
                    min = distance[mainId];
                    minNodeId = nodeId;
                    if(mydistance > source_destination_distance){  //テーブル内にBroadcastを受信したのが自分のみ
                      min = distance[mainId];
                      minNodeId = nodeId;
                    }
                  }
                }
              } else if(distance[mainId] == 0){
                //printf("arrived destination node\n");
                break;
              } else if(distance[mainId] == min){
                //printf("Emergency node\n");
              } else {
                //printf("node[%d] is too far\n",nodeId);
              }
        
              //if(itr == table.end()) break;

              itr++;
              itr2++;
              mainId++;

              if(itr == table.end()) break;    
            }
            //printf("路側機minNodeId = %d\n",minNodeId);
          }

          if(table3.size()>0){
            while(1){  //車両
              nodeId3 = itr7->first;
              //printf("nodeId3 = %d\n",nodeId3);
              uint16_t pos_x = itr7->second;
              //printf("pos_x = %d\n",pos_x);
              x4[nodeId3] = (e_dst_x-pos_x)*(e_dst_x-pos_x);
              //printf("nodeId3 = %d\n distance = %d\n",nodeId3,x4[nodeId3]);

        

              nodeId4 = itr8->first;
              //printf("nodeId2 = %d\n",nodeId2);
              uint16_t pos_y = itr8->second;
              //printf("pos_y = %d\n",pos_y);
              y4[nodeId4] = (e_dst_y-pos_y)*(e_dst_y-pos_y);
        
              distance2[mainId2] = x4[nodeId3] + y4[nodeId4]; //テーブル中の各ノードと目的までの距離
              //printf("nodeId3 = %d\ndistance = %d\n",nodeId3,distance2[mainId2]);
        

              if(distance2[mainId2] <= min2 ){  //目的ノードではない
                if(mydistance < distance2[mainId2]){ //自分が目的ノードに近い
                  min2 = mydistance;
                  minNodeId3 = m_ipv4->GetObject<Node> ()->GetId ();
                  //printf("min2 = %d\n",min2);
                  //printf("minNodeId3 = %d\n",minNodeId3);
                } else if(mydistance == distance2[mainId2]) { //同距離のノードが存在している場合
                  //minNodeId3 = m_ipv4->GetObject<Node> ()->GetId ();
                } else {  //テーブル中の他のノードが目的ノードに近いとき
                  if(source_other_distance2[mainId2]<10000){ //通信範囲内なら
                    min2 = distance2[mainId2];
                    minNodeId3 = nodeId3;
                    if(mydistance > source_destination_distance){  //テーブル内にBroadcastを受信したのが自分のみ
                      min2 = distance2[mainId2];
                      minNodeId3 = nodeId3;
                    }
                  }
                }
              } else if(distance2[mainId2] == 0){
                //printf("arrived destination node\n");
                break;
              } else if(distance2[mainId2] == min2){
                //printf("Emergency node\n");
              } else {
                //printf("node[%d] is too far\n",nodeId3);
              }
 
              itr7++;
              itr8++;
              mainId2++;
              if(itr7 == table3.end()) break;    
            }
            //if(minNodeId3==0)
              //minNodeId3=m_ipv4->GetObject<Node> ()->GetId ();
            //printf("車両minNodeId3 = %d\n",minNodeId3);
          }
          if(minNodeId==0)
            minNodeId=m_ipv4->GetObject<Node> ()->GetId ();
          if(minNodeId3==0)
            minNodeId3=m_ipv4->GetObject<Node> ()->GetId ();
          printf("路側機minNodeId = %d\n",minNodeId);
          printf("車両minNodeId3 = %d\n",minNodeId3);
          //printf("min = %d\n",min);


          printf("instruction=%d\n",instruction);
          if(minNodeId!=destination){ //目的地じゃないなら
            if(instruction==0){
              if(minNodeId>=RSU){ //路側機なら 
                if(m_ipv4->GetObject<Node> ()->GetId () == minNodeId){ 
                  //printf("minNodeId = %d\n",minNodeId);
                  //printf("min = %d\n",min);
                  SendBroadcast2(hop+1);
                }
                if(minNodeId == 0){                
                  SendBroadcast2(hop+1);
                }
              }
            }else{
              if(minNodeId3<emergencycar){ //一般車両なら
                if(m_ipv4->GetObject<Node> ()->GetId () == minNodeId3){ 
                  //printf("minNodeId3 = %d\n",minNodeId3);
                  //printf("min2 = %d\n",min2);
                  if(mypos_x >= source_pos_x && mypos_y >= source_pos_y)
                    SendBroadcast2(hop+1);
                }
                if(minNodeId3 == 0){
                  if(mypos_x >= source_pos_x && mypos_y >= source_pos_y)
                    SendBroadcast2(hop+1);
                }
              }
            }
          }
        //}
      //}
    }
  }
  Ptr<MobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<MobilityModel>();
  Vector pos = mobility->GetPosition ();

  uint16_t mynodeId = m_ipv4->GetObject<Node> ()->GetId ();
  auto itr55 = receive_node_table.find(mynodeId);

  if(itr55 != receive_node_table.end()){
    //なにもしない
    //printf("bbbbbbbbbbbb");
  } else {
    if(mynodeId != emergencycar){ //緊急車両じゃないなら
      if(pos.x >380 && pos.x < 420){
        receive_node_table.insert(std::make_pair(mynodeId, 0));
      }
      if(pos.x <=380){
        if(pos.y >0 && pos.y < 20){
          receive_node_table.insert(std::make_pair(mynodeId, 0));
        }
      }
      if(pos.x >= 420){
        if(pos.y > 880 && pos.y <920){
          receive_node_table.insert(std::make_pair(mynodeId, 0));
        }
      }
      uint16_t receive_node_table_size = receive_node_table.size();
      //printf("receive_node_table_size = %d\n",receive_node_table_size);
      if(receive_node_table_size >= 1){
        receive_node_size++;
      }
    }
  }
  break;
}
}
}

void
KitagawaRoutingProtocol::SendBroadcast (void) //緊急車両送信開始
{

  sequence++;
 //printf("SendBroadcast\n");
 std::cout<<"In send emergency car(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n";

 
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

      //隣接車両がx台以上y台未満なら路側機がBroadcast
      uint8_t temp = table3.size();
      if(temp<1 || temp>3){
        broadcastheader.SetInstruction(0); //NHT = RSU
      }else{
        broadcastheader.SetInstruction(1); //NHT = Car
      }
      //broadcastheader.SetInstruction(0);
      
      //ピュアフラッディング
      //broadcastheader.SetInstruction(1);
      
      broadcastheader.SetHopCount(1);
      broadcastheader.SetSequence(sequence);

      packet->AddHeader (broadcastheader);

///////////////////////////提案手法,比較手法でTYPEを決める 変更///////////////////////////////////////

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
      //printf("%d\n",m_ipv4->GetObject<Node> ()->GetId ());
        
     }
  //sequence++;
  number_of_data_in_emergency_route++;
  //EmergencyCourseChange();
}


void
KitagawaRoutingProtocol::SendBroadcast2(uint16_t hop)
{
  //printf("SendBroadcast2\n");

    //std::cout<<"In send2(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n";


    //printf("sequence = %d\n",sequence);
    //printf("hop = %d\n",hop);
    //if(hop<21){
    uint16_t mynodeId = m_ipv4->GetObject<Node> ()->GetId ();
    auto itr77 = sendnumbertable.find(mynodeId);
    //uint8_t table_size = sendnumbertable.size();

    //printf("table_size = %d\n",table_size);

    if(itr77 != sendnumbertable.end()){
      //なにもしない
      //printf("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n");
    } else {
      std::cout<<"\n\n\nIn send2(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n\n\n";
      sendnumbertable.insert(std::make_pair(mynodeId, 0));
      //printf("ccccccccccccccccccccccccccccccccc\n");
      uint16_t mypos_x, mypos_y;
      uint16_t my_x, my_y;
      uint32_t mydistance = 0;

      //車両用変数
      uint16_t nodeId3,nodeId4;
      uint32_t x4[BUFFER_SIZE], y4[BUFFER_SIZE], distance2[BUFFER_SIZE];
      auto mainId2 = 0;
   
      //車両テーブル
      auto itr7 = table3.begin();
      auto itr8 = table4.begin();

      Ptr<MobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<MobilityModel>();
      Vector mypos = mobility->GetPosition ();

      mypos_x = mypos.x;
      mypos_y = mypos.y;

      my_x = (e_dst_x-mypos_x)*(e_dst_x-mypos_x); //目的ノードによって
      my_y = (e_dst_y-mypos_y)*(e_dst_y-mypos_y); //目的ノードによって

      mydistance = my_x + my_y;
      //printf("mydistance = %d\n",mydistance);

      if(table3.size()>0){
        while(1){  //車両
          nodeId3 = itr7->first;
          //printf("nodeId3 = %d\n",nodeId3);
          uint16_t pos_x = itr7->second;
          //printf("pos_x = %d\n",pos_x);
          x4[nodeId3] = (e_dst_x-pos_x)*(e_dst_x-pos_x);  //目的地によって
          //printf("nodeId3 = %d\n distance = %d\n",nodeId3,x4[nodeId]);


          nodeId4 = itr8->first;
          //printf("nodeId2 = %d\n",nodeId2);
          uint16_t pos_y = itr8->second;
          //printf("pos_y = %d\n",pos_y);
          y4[nodeId4] = (e_dst_y-pos_y)*(e_dst_y-pos_y);  //目的地によって
        
          distance2[mainId2] = x4[nodeId3] + y4[nodeId4]; //テーブル中の各ノードと目的までの距離
          //printf("nodeId3 = %d\n distance2 = %d\n",nodeId3,distance2[mainId2]);

          itr7++;
          itr8++;
          mainId2++;
          if(itr7 == table3.end()) break;    
        }
        //printf("車両minNodeId3 = %d\n",minNodeId3);
      }

      //printf("mainId2 = %d\n",mainId2);

      uint8_t temp = 0;
 
      for(uint8_t i=0;i<mainId2;i++){
        if(distance2[i] != 0){
          if(distance2[i]<mydistance)
            temp = temp + 1;
            //printf("mydistance = %d\n",mydistance);
            //printf("distance = %d\n",distance2[i]);
        }
      }
      //printf("temp = %d\n",temp);

      for (std::map<Ptr<Socket>, Ipv4InterfaceAddress>::const_iterator i =
         m_socketAddresses.begin (); i != m_socketAddresses.end (); i++)
      {
        Ptr<Socket> socket = i->first;
        Ipv4InterfaceAddress iface = i->second;
        BroadcastHeader broadcastheader;
        broadcastheader.SetSource(m_ipv4->GetObject<Node> ()->GetId ());
        //隣接車両がx台以上y台未満なら路側機がBroadcast
        //提案手法    
        if(temp<1 || temp>3){
          broadcastheader.SetInstruction(0); //NHT = RSU
        }else{
          broadcastheader.SetInstruction(1); //NHT = Car
        }
        
        //比較手法
        //broadcastheader.SetInstruction(1);


        broadcastheader.SetHopCount(hop);
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

      uint16_t mynodeId = m_ipv4->GetObject<Node> ()->GetId ();
      if(mynodeId != emergencycar){ //緊急車両じゃないなら
        /*if(mypos.x > 380 && mypos.x < 420){
          number_of_data_in_emergency_route++;
        }
        if(mypos.x <= 380){
          if(mypos.y > 0 && mypos.y <20){
            number_of_data_in_emergency_route++;
          }
        }
        if(mypos.x >= 420){
          if(mypos.y > 880 && mypos.y < 920){
            number_of_data_in_emergency_route++;
          }
        }*/
        number_of_data_in_emergency_route++; 
      }
    } 
}

void
KitagawaRoutingProtocol::SendHello()
{
  Ptr<MobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<MobilityModel>();
  Vector pos = mobility->GetPosition ();

  uint16_t simulator_time = Simulator::Now().GetSeconds();

  if(pos.x != 2000){

  if(simulator_time<=100){

  //if(pos.x != 2000){

  //std::cout<<"In sendhello(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n";
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

    if(m_ipv4->GetObject<Node> ()->GetId () <emergencycar){ //一般車両なら
      //std::cout << direction_table.size()<<"\n";
      if(direction_table.size()<= 1)
        direction_table.insert(std::make_pair(pos.x, pos.y));
    }

    TypeHeader tHeader (HELLOTYPE);
    packet->AddHeader (tHeader);
    if(m_ipv4->GetObject<Node> ()->GetId () == destination){ //目的地なら
      e_dst_x = pos.x;
      e_dst_y = pos.y;
    }

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
  }else if(simulator_time>100 && simulator_time<200){
     if(pos.y>100){
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

    if(m_ipv4->GetObject<Node> ()->GetId () <emergencycar){ //一般車両なら
      //std::cout << direction_table.size()<<"\n";
      if(direction_table.size()<= 1)
        direction_table.insert(std::make_pair(pos.x, pos.y));
    }

    TypeHeader tHeader (HELLOTYPE);
    packet->AddHeader (tHeader);

    if(m_ipv4->GetObject<Node> ()->GetId () == destination){ //目的地なら
      e_dst_x = pos.x;
      e_dst_y = pos.y;
    }

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
  }else{
     if(pos.y >600){
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

    if(m_ipv4->GetObject<Node> ()->GetId () <emergencycar){ //一般車両なら
      //std::cout << direction_table.size()<<"\n";
      if(direction_table.size()<= 1)
        direction_table.insert(std::make_pair(pos.x, pos.y));
    }

    TypeHeader tHeader (HELLOTYPE);
    packet->AddHeader (tHeader);

    if(m_ipv4->GetObject<Node> ()->GetId () == destination){ //目的地なら
      e_dst_x = pos.x;
      e_dst_y = pos.y;
    }

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

  if(m_ipv4->GetObject<Node> ()->GetId () == emergencycar){ //緊急車両なら
    Ptr<ConstantVelocityMobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<ConstantVelocityMobilityModel>();
    Vector pos = mobility->GetPosition ();

    if(pos.x >= 100){
      if(count == 0){
        count++;
        printf("\n1\n");
      }
    }
    if(pos.x >= 200){
      if(count == 1){
        count++;
        printf("2\n");
      }
    }
    if(pos.x >= 300){
      if(count == 2){
        count++;
        printf("3\n");
      }
    }          
    if(pos.x >= 400){
      if(count == 3){
        mobility->SetPosition(Vector(400,0,0));
        mobility->SetVelocity(Vector(0,12,0));
        count++;
        printf("第一曲がり角3.5\n");
      }
    }
    if(pos.y >= 100){
      if(count == 4){
        count++;
        printf("4\n");
      }
    }
    if(pos.y >= 200){
      if(count == 5){
        count++;
        printf("5\n");
      }
    }
    if(pos.y >= 300){
      if(count == 6){
        count++;
        printf("6\n");
      }
    }
    if(pos.y >= 400){
      if(count == 7){
        count++;
        printf("7\n");
      }
    }
    if(pos.y >= 450){
      if(count == 8){
        count++;
        printf("中間地点8\n");
      }
    }
    if(pos.y >= 500){
      if(count == 9){
        count++;
        printf("9\n");
      }
    }
    if(pos.y >= 600){
      if(count == 10){
        count++;
        printf("10\n");
      }
    }
    if(pos.y >= 700){
      if(count == 11){
        count++;
        printf("11\n");
      }
    }
    if(pos.y >= 800){
      if(count == 12){
        count++;
        printf("12\n");
      }
    }
    if(pos.y >= 900){
      if(count == 13){
        mobility->SetPosition(Vector(400,900,0));
        mobility->SetVelocity(Vector(12,0,0));
        count++;
        printf("第二曲がり角13\n");
      }      
    }
    if(pos.x >= 500){
      if(count == 14){
        count++;
        printf("14\n");
      }
    }
    if(pos.x >= 600){
      if(count == 15){
        count++;
        printf("15\n");
      }
    }
    if(pos.x >= 700){
      if(count == 16){
        count++;
        printf("16\n");
      }
    }
    if(pos.x >= 800){
      if(count == 17){
        count++;
        printf("17\n");
      }
    }
    if(pos.x >= 900){
      if(count == 18){
        count++;
        printf("18\n");
      }
    }
        //緊急車両の目的地到着時間計測
    if(pos.x >= 950){
      if(count == 19){
        arrived_destination_time = Simulator::Now().GetSeconds();
        count++;
      }
    }
    if(pos.x >=1000){
      if(count == 20){
        mobility->SetPosition(Vector(1000,900,0));
        mobility->SetVelocity(Vector(0,0,0));
        printf("\n\n\narrived desitinetion!!\n\n\n\n");
        count++;
      }
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

  if(m_ipv4->GetObject<Node> ()->GetId () == emergencycar){ //緊急車両なら
  
    //printf("nodeId=%d\n",nodeId);
    if(nodeId >=RSU){ //路側機ノード
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
    //printf("nodeId=%d\n",nodeId);
    if(nodeId >=RSU){ //路側機ノード
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
    //変更 通信不搭載車の設定-----------
      for(uint16_t z=0;z<125;z++){
    //---------------------------------
        if(connectedcar[z] == nodeId){
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
}

void
KitagawaRoutingProtocol::EmergencyCourseChange2()
{
  Ptr<ConstantVelocityMobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<ConstantVelocityMobilityModel>();
  Vector mypos = mobility->GetPosition ();

  if(mypos.y==0 || mypos.y==900){
    mobility->SetVelocity(Vector(12,0,0));
  }else{
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
  std::cout<<"In sendpureflood(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n";
  for (std::map<Ptr<Socket>, Ipv4InterfaceAddress>::const_iterator j = m_socketAddresses.begin (); j
       != m_socketAddresses.end (); ++j)
    {

      Ptr<Socket> socket = j->first;
      Ipv4InterfaceAddress iface = j->second;
      Ptr<Packet> packet = Create<Packet> ();

      BroadcastHeader broadcastheader;
      broadcastheader.SetSequence(sequence);
      broadcastheader.SetHopCount(hop);
      broadcastheader.SetSource(m_ipv4->GetObject<Node> ()->GetId ());

      packet->AddHeader (broadcastheader);

      TypeHeader tHeader (BROADCASTTYPE3);
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
  number_of_data_in_emergency_route++;
}

void
KitagawaRoutingProtocol::RecvPureFlooding(Ptr<Packet> packet, Ipv4Address receiver, Ipv4Address sender)
{
  //std::cout<<"In recvPureBroadcast(Node "<< m_ipv4->GetObject<Node> ()->GetId ()<<")\n";
  if(m_ipv4->GetObject<Node> ()->GetId ()==destination){ //目的地なら
    printf("arrived data\n\n");
  }else{
    if(m_ipv4->GetObject<Node> ()->GetId () != emergencycar){ //緊急車両じゃないなら
      BroadcastHeader broadcastheader;
      packet->RemoveHeader(broadcastheader);
      uint8_t sequence = broadcastheader.GetSequence();
      uint16_t mynodeId = m_ipv4->GetObject<Node>()->GetId();
      uint16_t hop = broadcastheader.GetHopCount();
      //uint8_t source = broadcastheader.GetSource();

      //printf("hop = %d\n",hop);
      //printf("Sequence = %d\n",sequence);
      //printf("source = %d\n",source);

      //std::cout << sendnumbertable.size()<<"\n";

      //auto itr = sendnumbertable.find(mynodeId);
      //auto itr2 = sendnumbertable.find(sequence);


      if(sequence==1){
        auto itr = sendnumbertable.find(mynodeId);
        auto itr2 = sendnumbertable.find(sequence);

        if(itr != sendnumbertable.end()){ //ある
          //printf("111111111111111111\n");
          if(itr2 != sendnumbertable.end()){ //ある
            /*printf("222222222222222222222222222222\n");
            sendnumbertable.erase(itr2);
            sendnumbertable.insert(std::make_pair(mynodeId, sequence)); 
            SendPureFlooding(hop+1);
          } else { //ない
            printf("444444444444444444444444444444\n");    
            //sendnumbertable.insert(std::make_pair(mynodeId, sequence)); 
            //SendPureFlooding(hop+1);*/
          }
        } else{ //ない
            //printf("333333333333333333333333333333333333\n");
            sendnumbertable.insert(std::make_pair(mynodeId, sequence));
            hop++;
            SendPureFlooding(hop+1);
        }
      }else{
        auto itr3 = sendnumbertable.find(mynodeId);
        auto itr4 = sendnumbertable.find(sequence);
        sendnumbertable.clear();
        //std::cout << sendnumbertable.size()<<"\n";
        if(itr3 != sendnumbertable.end()){ //ある
          //printf("55555555555555555555555\n");
          if(itr4 != sendnumbertable.end()){ //ある
            //printf("222222222222222222222222222222\n");
            //sendnumbertable.erase(itr2);
            //sendnumbertable.insert(std::make_pair(mynodeId, sequence)); 
            //SendPureFlooding(hop+1);
          //} else { //ない
            //printf("444444444444444444444444444444\n");    
            sendnumbertable.insert(std::make_pair(mynodeId, sequence)); 
            SendPureFlooding(hop+1);
          }
        } else{ //ない
            //printf("66666666666666666666666\n");
            //sendnumbertable.insert(std::make_pair(mynodeId, sequence));
            //SendPureFlooding(hop+1);
        }
      }
    }
  }


  uint16_t mynodeId = m_ipv4->GetObject<Node> ()->GetId ();
  auto itr77 = recvnumbertable.find(mynodeId);

  if(itr77 != recvnumbertable.end()){
    if(m_ipv4->GetObject<Node> ()->GetId ()<emergencycar){ //一般車両なら
      Ptr<ConstantVelocityMobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<ConstantVelocityMobilityModel>();
      //緊急車両目的到着時間測定 変更--------------------------------
      mobility->SetPosition(Vector(2000,2000,0));
      //------------------------------------------------------------
    }
  } else {
    recvnumbertable.insert(std::make_pair(mynodeId, 0));
        
    //一般車両受信率
    if(m_ipv4->GetObject<Node> ()->GetId ()<emergencycar){ //一般車両なら
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





  /*Ptr<MobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<MobilityModel>();
  Vector pos = mobility->GetPosition ();
  
  uint16_t mynodeId = m_ipv4->GetObject<Node> ()->GetId ();
  auto itr55 = receive_node_table.find(mynodeId);

  if(itr55 != receive_node_table.end()){
    //なにもしない
    //printf("bbbbbbbbbbbb\n");
  } else {
    if(mynodeId != emergenycar){ //緊急車両じゃないなら
      if(pos.x > 380 && pos.x < 420){
        receive_node_table.insert(std::make_pair(mynodeId, 0));
      }
      if(pos.x <= 380){
        if(pos.y > 0 && pos.y <20){
          receive_node_table.insert(std::make_pair(mynodeId, 0));
        }
      }
      if(pos.x >=420){
        if(pos.y > 880 && pos.y <920){
          receive_node_table.insert(std::make_pair(mynodeId, 0));
        }
      }
      //printf("receive_node_table_size = %d\n",receive_node_table_size);
      if(receive_node_table_size >= 1){
        receive_node_size++;
      }
    }
  }*/
  

}

} //namespace kitagawa
} //namespace ns3
