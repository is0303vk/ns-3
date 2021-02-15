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
 * Author: Yuki Kitagawa <ramonet@fc.ritsumei.ac.jp>
 *         Ritsumeikan University, Shiga, Japan
 */


//有線無線併用型
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <time.h> 

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/kitagawa-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"

#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"
  


NS_LOG_COMPONENT_DEFINE ("KitagawaProtocolMinimum");

using namespace ns3;


//uint16_t port = 625;
Ptr<ConstantVelocityMobilityModel> cvmm;


int
main (int argc, char *argv[])
{
//変更----------------------------------------------
  uint16_t Car = 1000;
//--------------------------------------------------
  uint16_t EmergencyCar = 1;
  uint16_t CarNodes = Car + EmergencyCar;   //車両数,緊急車両
  uint16_t csma_RSUNodes = 15;   //路側機 16台(StaNode + ApNode)
  uint16_t adjustnodes = 2;


  RngSeedManager::SetSeed(22);
  RngSeedManager::SetRun (7);

  ////////////////////// CREATE NODES ////////////////////////////////////
  NodeContainer carnodes;
  carnodes.Create(CarNodes);
  std::cout<<"CarNodes created\n";


  NodeContainer p2pNodes;
  p2pNodes.Create(2);

  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute("DataRate",StringValue("100Mbps"));
  pointToPoint.SetChannelAttribute("Delay",StringValue("1ms"));

  NetDeviceContainer p2pDevices;
  p2pDevices = pointToPoint.Install(p2pNodes);

  NodeContainer csma_rsunodes;
  csma_rsunodes.Add(p2pNodes.Get(0));
  csma_rsunodes.Create(csma_RSUNodes);  

  NodeContainer ApNode = p2pNodes.Get(1);
  NodeContainer ApNode1 = csma_rsunodes.Get(1);
  NodeContainer ApNode2 = csma_rsunodes.Get(2);  
  NodeContainer ApNode3 = csma_rsunodes.Get(3);
  NodeContainer ApNode4 = csma_rsunodes.Get(4);
  NodeContainer ApNode5 = csma_rsunodes.Get(5);
  NodeContainer ApNode6 = csma_rsunodes.Get(6);  
  NodeContainer ApNode7 = csma_rsunodes.Get(7);
  NodeContainer ApNode8 = csma_rsunodes.Get(8);
  NodeContainer ApNode9 = csma_rsunodes.Get(9);
  NodeContainer ApNode10 = csma_rsunodes.Get(10);  
  NodeContainer ApNode11 = csma_rsunodes.Get(11);
  NodeContainer ApNode12 = csma_rsunodes.Get(12);
  NodeContainer ApNode13 = csma_rsunodes.Get(13);
  NodeContainer ApNode14 = csma_rsunodes.Get(14); 
  NodeContainer ApNode15 = csma_rsunodes.Get(15);    
  ///////////////////// CREATE DEVICES ////////////////////////////////////

  UintegerValue ctsThr = (true ? UintegerValue (100) : UintegerValue (2200));
  Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", ctsThr);

  // A high number avoid drops in packet due to arp traffic.
  Config::SetDefault ("ns3::ArpCache::PendingQueueSize", UintegerValue (400));

  CsmaHelper csma;
  csma.SetChannelAttribute("DataRate",StringValue("100Mbps"));
  csma.SetChannelAttribute("Delay",StringValue("1ms"));


  NetDeviceContainer wire_devices;
  wire_devices = csma.Install(csma_rsunodes);

  //////wirelessの設定////
  NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default();
  //wifi80211pMac.SetType ("ns3::AdhocWifiMac");

  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel;

  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel","MaxRange", DoubleValue(100));

  wifiPhy.SetChannel (wifiChannel.Create ());

  Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default();
  wifi80211p.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("OfdmRate6MbpsBW10MHz"), "RtsCtsThreshold", UintegerValue (0),"ControlMode",StringValue("OfdmRate6MbpsBW10MHz"));

  NetDeviceContainer car_wireless_devices,RSU_wireless_devices,Emer_wireless_devices;
  car_wireless_devices = wifi80211p.Install (wifiPhy, wifi80211pMac, carnodes);

  RSU_wireless_devices = wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode);
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode1));
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode2));
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode3));
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode4));  
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode5));
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode6));
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode7));
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode8));  
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode9));
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode10));
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode11));  
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode12));
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode13));
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode14));
  RSU_wireless_devices.Add(wifi80211p.Install (wifiPhy, wifi80211pMac, ApNode15));     

  //device.Add(〜);
   std::cout<<"Devices installed\n";
   
   ////////////////////////   MOBILITY  ///////////////////////////////////////////
  

     // Create mobility
  MobilityHelper mobility;
  Ptr<ConstantVelocityMobilityModel> cvmm;
  Ptr<ConstantPositionMobilityModel> cpmm;

 
  //緊急車両
  mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  mobility.Install(carnodes.Get(1));
  cvmm = carnodes.Get(1)->GetObject<ConstantVelocityMobilityModel> ();
  cvmm->SetPosition(Vector(0,500,0));
  cvmm->SetVelocity(Vector(12,0,0));

  

  //RSU
  //消防署ノード
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(csma_rsunodes.Get(0));
  cpmm = csma_rsunodes.Get(0)->GetObject<ConstantPositionMobilityModel> ();
  cpmm->SetPosition(Vector(0,100,0));


  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(ApNode.Get(0));
  cpmm = ApNode.Get(0)->GetObject<ConstantPositionMobilityModel> ();
  cpmm->SetPosition(Vector(100,500,0));

  for(uint16_t i=1;i<=3;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(csma_rsunodes.Get(i));
    cpmm = csma_rsunodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*300+100,500,0));
  }

  for(uint16_t i=4;i<=7;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(csma_rsunodes.Get(i));
    cpmm = csma_rsunodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector((i-4)*300+100,800,0));
  }

  for(uint16_t i=8;i<=11;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(csma_rsunodes.Get(i));
    cpmm = csma_rsunodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector((i-8)*300+100,1100,0));
  }

  for(uint16_t i=12;i<=14;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(csma_rsunodes.Get(i));
    cpmm = csma_rsunodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector((i-12)*300+100,1400,0));
  }

  //目的ノード
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(csma_rsunodes.Get(15));
  cpmm = csma_rsunodes.Get(15)->GetObject<ConstantPositionMobilityModel> ();
  cpmm->SetPosition(Vector(1000,1400,0));




  //uint16_t now = (uint16_t)time(0);
  srand(14);

  /*for(uint16_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobility.Install(carnodes.Get(i));
    cvmm = carnodes.Get(i)->GetObject<ConstantVelocityMobilityModel> ();
    cvmm->SetPosition(Vector(i*25,0,0));
    cvmm->SetVelocity(Vector(4,0,0));
  }*/

  //車両
  int e = rand()%9;
  int f = rand()%11;

  mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  mobility.Install(carnodes.Get(0));
  cvmm = carnodes.Get(0)->GetObject<ConstantVelocityMobilityModel> ();
  cvmm->SetPosition(Vector(e*100+100,f*100+500,0));
  cvmm->SetVelocity(Vector(12,0,0));
  

//変更---------------------------------------------------
  for(uint16_t i=2;i<=Car;i++){
//-------------------------------------------------------
    mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
    mobility.Install(carnodes.Get(i));
    cvmm = carnodes.Get(i)->GetObject<ConstantVelocityMobilityModel> ();

    int x = rand()%2;
    int y = rand()%10;
    int z = rand()%10;
    int k = rand()%2;

    uint16_t a = rand()%1000+51; //横軸車両の初期位置
    uint16_t b = rand()%900+1; //縦軸車両の初期位置

    if(x==0){
      switch(y){
        case 0:
          cvmm->SetPosition(Vector(a,500,0));
          if(k==0){
            cvmm->SetVelocity(Vector(12,0,0));
          }else{
            cvmm->SetVelocity(Vector(-12,0,0));
          }
          break;
        case 1:
          cvmm->SetPosition(Vector(a,600,0));
          if(k==0){
            cvmm->SetVelocity(Vector(12,0,0));
          }else{
            cvmm->SetVelocity(Vector(-12,0,0));
          }
          break;
        case 2:
          cvmm->SetPosition(Vector(a,700,0));
          if(k==0){
            cvmm->SetVelocity(Vector(12,0,0));
          }else{
            cvmm->SetVelocity(Vector(-12,0,0));
          }
          break;
        case 3:
          cvmm->SetPosition(Vector(a,800,0));
          if(k==0){
            cvmm->SetVelocity(Vector(12,0,0));
          }else{
            cvmm->SetVelocity(Vector(-12,0,0));
          }
          break;
        case 4:
          cvmm->SetPosition(Vector(a,900,0));
          if(k==0){
            cvmm->SetVelocity(Vector(12,0,0));
          }else{
            cvmm->SetVelocity(Vector(-12,0,0));
          }
          break;
        case 5:
          cvmm->SetPosition(Vector(a,1000,0));
          if(k==0){
            cvmm->SetVelocity(Vector(12,0,0));
          }else{
            cvmm->SetVelocity(Vector(-12,0,0));
          }
          break;
        case 6:
          cvmm->SetPosition(Vector(a,1100,0));
          if(k==0){
            cvmm->SetVelocity(Vector(12,0,0));
          }else{
            cvmm->SetVelocity(Vector(-12,0,0));
          }
          break;
        case 7:
          cvmm->SetPosition(Vector(a,1200,0));
          if(k==0){
            cvmm->SetVelocity(Vector(12,0,0));
          }else{
            cvmm->SetVelocity(Vector(-12,0,0));
          }
          break;
        case 8:
          cvmm->SetPosition(Vector(a,1300,0));
          if(k==0){
            cvmm->SetVelocity(Vector(12,0,0));
          }else{
            cvmm->SetVelocity(Vector(-12,0,0));
          }
          break;        
        case 9:
          cvmm->SetPosition(Vector(a,1400,0));
          if(k==0){
            cvmm->SetVelocity(Vector(12,0,0));
          }else{
            cvmm->SetVelocity(Vector(-12,0,0));
          }
          break;
        default:
          break;
      }
    }
    if(x==1){
      switch(z){
        case 0:
          cvmm->SetPosition(Vector(100,b+500,0));
          if(k==0){
            cvmm->SetVelocity(Vector(0,12,0));
          }else{
            cvmm->SetVelocity(Vector(0,-12,0));
          }
          break;
        case 1:
          cvmm->SetPosition(Vector(200,b+500,0));
          if(k==0){
            cvmm->SetVelocity(Vector(0,12,0));
          }else{
            cvmm->SetVelocity(Vector(0,-12,0));
          }
          break;
        case 2:
          cvmm->SetPosition(Vector(300,b+500,0));
          if(k==0){
            cvmm->SetVelocity(Vector(0,12,0));
          }else{
            cvmm->SetVelocity(Vector(0,-12,0));
          }
          break;
        case 3:
          cvmm->SetPosition(Vector(400,b+500,0));
          if(k==0){
            cvmm->SetVelocity(Vector(0,12,0));
          }else{
            cvmm->SetVelocity(Vector(0,-12,0));
          }
          break;
        case 4:
          cvmm->SetPosition(Vector(500,b+500,0));
          if(k==0){
            cvmm->SetVelocity(Vector(0,12,0));
          }else{
            cvmm->SetVelocity(Vector(0,-12,0));
          }
          break;
        case 5:
          cvmm->SetPosition(Vector(600,b+500,0));
          if(k==0){
            cvmm->SetVelocity(Vector(0,12,0));
          }else{
            cvmm->SetVelocity(Vector(0,-12,0));
          }
          break;
        case 6:
          cvmm->SetPosition(Vector(700,b+500,0));
          if(k==0){
            cvmm->SetVelocity(Vector(0,12,0));
          }else{
            cvmm->SetVelocity(Vector(0,-12,0));
          }
          break;
        case 7:
          cvmm->SetPosition(Vector(800,b+500,0));
          if(k==0){
            cvmm->SetVelocity(Vector(0,12,0));
          }else{
            cvmm->SetVelocity(Vector(0,-12,0));
          }
          break;
        case 8:
          cvmm->SetPosition(Vector(900,b+500,0));
          if(k==0){
            cvmm->SetVelocity(Vector(0,12,0));
          }else{
            cvmm->SetVelocity(Vector(0,-12,0));
          }
          break;
        case 9:
          cvmm->SetPosition(Vector(1000,b+500,0));
          if(k==0){
            cvmm->SetVelocity(Vector(0,12,0));
          }else{
            cvmm->SetVelocity(Vector(0,-12,0));
          }
          break;          
        default:
          break;
      }
    }
  }

  NodeContainer adjust_nodes;
  adjust_nodes.Create(adjustnodes);

  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(adjust_nodes.Get(0));
  cpmm = adjust_nodes.Get(0)->GetObject<ConstantPositionMobilityModel> ();
  cpmm->SetPosition(Vector(500,2000,0));

  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(adjust_nodes.Get(1));
  cpmm = adjust_nodes.Get(1)->GetObject<ConstantPositionMobilityModel> ();
  cpmm->SetPosition(Vector(2000,500,0));
  



  std::cout<<"mobility set \n";


  ////////////////////////   INTERNET STACK /////////////////////////

   KitagawaHelper kitagawaProtocol;

   Ipv4ListRoutingHelper listrouting;
   listrouting.Add(kitagawaProtocol, 10);

   InternetStackHelper stack;
   stack.SetRoutingHelper(listrouting);
   stack.Install (carnodes);
   stack.Install (csma_rsunodes);
   stack.Install (ApNode);

  
   Ipv4AddressHelper ipv4;
   NS_LOG_INFO ("Assign IP Addresses.");
 
   ipv4.SetBase ("10.1.0.0", "255.255.0.0");
   Ipv4InterfaceContainer p2pInterfaces;
   p2pInterfaces = ipv4.Assign(p2pDevices);

   ipv4.SetBase ("10.2.0.0", "255.255.0.0");
   Ipv4InterfaceContainer csmaInterfaces;
   csmaInterfaces = ipv4.Assign(wire_devices);

   ipv4.SetBase ("10.3.0.0", "255.255.0.0");
   ipv4.Assign(car_wireless_devices);
   ipv4.Assign(RSU_wireless_devices);

   std::cout<<"Internet Stack installed\n";

   //interfaces.Add(〜);
   
  
    AnimationInterface anim("animation18.xml");


    Simulator::Stop(Seconds (300));

    
    
    Simulator::Run ();



    // clean variables
    carnodes = NodeContainer();
    csma_rsunodes = NodeContainer();
    //interfaces = Ipv4InterfaceContainer ();
    //csma_interfaces = Ipv4InterfaceContainer ();
    car_wireless_devices = NetDeviceContainer ();
    wire_devices = NetDeviceContainer ();

    Simulator::Destroy ();
    std::cout<<"end of simulation\n";

}
