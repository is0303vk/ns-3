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


//路側機配置


#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/kitagawa-module.h"
#include "ns3/applications-module.h"
#include "ns3/netanim-module.h"
#include "ns3/packet-loss-counter.h"

#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"


#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <time.h> 




NS_LOG_COMPONENT_DEFINE ("KitagawaProtocol");

using namespace ns3;


//uint16_t port = 625;
Ptr<ConstantVelocityMobilityModel> cvmm;

uint16_t DropCount = 0;

void
DropTrace (std::string context, Ipv4Header const &header, Ptr< const Packet > packet, Ipv4L3Protocol::DropReason reason, Ptr<Ipv4> ipv4, uint32_t interface)
{
	/*
	 * Drop reasons Trace
	 *
	 * 1- DROP_TTL_EXPIRED 	      Packet TTL has expired.
	   2- DROP_NO_ROUTE 	      No route to host.
	   3- DROP_BAD_CHECKSUM 	  Bad checksum.
	   4- DROP_INTERFACE_DOWN 	  Interface is down so cannot send packet.
	   5- DROP_ROUTE_ERROR 	      Route error.
	   6- DROP_FRAGMENT_TIMEOUT   Fragment timeout exceeded.
	 */

    DropCount = DropCount+1;
    std::cout<<Simulator::Now().GetSeconds()<<" PACKET DROPPED " <<context <<" Reason:  "<<reason<< std::endl;
}


int
main (int argc, char *argv[])
{ 
   uint8_t EmergencyCar = 1;
   uint8_t RSUNodes = 100; //(無線型)
   //uint8_t RSUNodes = 16; //(有線併用型)

   uint16_t numNodes = EmergencyCar + RSUNodes;


   //int pktSize = 1024;  //packet Size in bytes


   RngSeedManager::SetSeed(22);
   RngSeedManager::SetRun (7);

   ////////////////////// CREATE NODES ////////////////////////////////////
   NodeContainer nodes;
   nodes.Create(numNodes);
   std::cout<<"Nodes created\n";

   ///////////////////// CREATE DEVICES ////////////////////////////////////

   UintegerValue ctsThr = (true ? UintegerValue (100) : UintegerValue (2200));
   Config::SetDefault ("ns3::WifiRemoteStationManager::RtsCtsThreshold", ctsThr);

   // A high number avoid drops in packet due to arp traffic.
   Config::SetDefault ("ns3::ArpCache::PendingQueueSize", UintegerValue (400));

  NqosWaveMacHelper wifi80211pMac = NqosWaveMacHelper::Default();
  //wifi80211pMac.SetType ("ns3::AdhocWifiMac");

  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  YansWifiChannelHelper wifiChannel;

  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");
  //wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel","MaxRange", DoubleValue(75));
  wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel","MaxRange", DoubleValue(110));

  wifiPhy.SetChannel (wifiChannel.Create ());

  Wifi80211pHelper wifi80211p = Wifi80211pHelper::Default();
  wifi80211p.SetRemoteStationManager ("ns3::ConstantRateWifiManager", "DataMode", StringValue ("OfdmRate6MbpsBW10MHz"), "RtsCtsThreshold", UintegerValue (0),"ControlMode",StringValue("OfdmRate6MbpsBW10MHz"));

  NetDeviceContainer devices;
  devices = wifi80211p.Install (wifiPhy, wifi80211pMac, nodes);



   std::cout<<"Devices installed\n";

   ////////////////////////   MOBILITY  ///////////////////////////////////////////

     // Create mobility
  MobilityHelper mobility;
  Ptr<ConstantVelocityMobilityModel> cvmm;
  Ptr<ConstantPositionMobilityModel> cpmm;

 
  //緊急車両
  mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  mobility.Install(nodes.Get(0));
  cvmm = nodes.Get(0)->GetObject<ConstantVelocityMobilityModel> ();
  cvmm->SetPosition(Vector(0,0,0));
  cvmm->SetVelocity(Vector(12,0,0));

  //路側機(有線併用型)
  /*
  for(uint8_t i=0;i<=3;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+1));
    cpmm = nodes.Get(i+1)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(100+i*300,0,0));
  }

  for(uint8_t i=0;i<=3;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+5));
    cpmm = nodes.Get(i+5)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(100+i*300,300,0));
  }  

  for(uint8_t i=0;i<=3;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+9));
    cpmm = nodes.Get(i+9)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(100+i*300,600,0));
  }

  for(uint8_t i=0;i<=2;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+13));
    cpmm = nodes.Get(i+13)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(100+i*300,900,0));
  }
  

 
  //目的ノード
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(nodes.Get(16));
  cpmm = nodes.Get(16)->GetObject<ConstantPositionMobilityModel> ();
  cpmm->SetPosition(Vector(1000,900,0));

  std::cout<<"mobility set \n";
  */


  //路側機(無線型)
  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i));
    cpmm = nodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,0,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+10));
    cpmm = nodes.Get(i+10)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,100,0));
  }  

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+20));
    cpmm = nodes.Get(i+20)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,200,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+30));
    cpmm = nodes.Get(i+30)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,300,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+40));
    cpmm = nodes.Get(i+40)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,400,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+50));
    cpmm = nodes.Get(i+50)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,500,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+60));
    cpmm = nodes.Get(i+60)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,600,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+70));
    cpmm = nodes.Get(i+70)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,700,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+80));
    cpmm = nodes.Get(i+80)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,800,0));
  }

  for(uint8_t i=1;i<=9;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+90));
    cpmm = nodes.Get(i+90)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,900,0));
  }
  //目的ノード
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(nodes.Get(100));
  cpmm = nodes.Get(100)->GetObject<ConstantPositionMobilityModel> ();
  cpmm->SetPosition(Vector(1000,900,0));

  std::cout<<"mobility set \n";


  ////////////////////////   INTERNET STACK /////////////////////////

   KitagawaHelper kitagawaProtocol;

   Ipv4ListRoutingHelper listrouting;
   listrouting.Add(kitagawaProtocol, 10);

   InternetStackHelper internet;
   internet.SetRoutingHelper(listrouting);
   internet.Install (nodes);
  
   Ipv4AddressHelper ipv4;
   NS_LOG_INFO ("Assign IP Addresses.");
   ipv4.SetBase ("10.1.0.0", "255.255.0.0");
   Ipv4InterfaceContainer interfaces;
   interfaces = ipv4.Assign (devices);
   std::cout<<"Internet Stack installed\n";

   
  
    AnimationInterface anim("animation99.xml");

    //wifiPhy.EnablePcapAll("wifiphy");    

    Simulator::Stop(Seconds (15));    
    
    //Config::Connect ("/NodeList/*/$ns3::Ipv4L3Protocol/Drop",MakeCallback (&DropTrace));

    Simulator::Run ();




    // clean variables
    nodes = NodeContainer();
    interfaces = Ipv4InterfaceContainer ();
    devices = NetDeviceContainer ();

    
    Simulator::Destroy ();
    printf("The Number Of Packet Drop is %d\n",DropCount);
    std::cout<<"end of simulation\n";

}
