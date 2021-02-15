/*
 *
 * Usage of ns2-mobility-trace:
 *
 *  ./waf --run "ns2-mobility-trace \
 *        --traceFile=src/mobility/examples/default.ns_movements
 *        --numNodes=2  --duration=100.0 --logFile=ns2-mobility-trace.log"
 *
 *  NOTE: ns2-traces-file could be an absolute or relative path. You could use the file default.ns_movements
 *        included in the same directory as the example file.
 *  NOTE 2: Number of nodes present in the trace file must match with the command line argument.
 *          Note that you must know it before to be able to load it.
 *  NOTE 3: Duration must be a positive number and should match the trace file. Note that you must know it before to be able to load it.
 */


#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <time.h>

#include "ns3/core-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/kitagawa-module.h"
#include "ns3/applications-module.h"
#include "ns3/ns2-mobility-helper.h"
#include "ns3/netanim-module.h"

#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("KitagawaProtocol");

// Prints actual position and velocity when a course change event occurs
/*static void
CourseChange (std::ostream *os, std::string foo, Ptr<const MobilityModel> mobility)
{
  Vector pos = mobility->GetPosition (); // Get position
  Vector vel = mobility->GetVelocity (); // Get velocity

  // Prints position and velocities
  *os << Simulator::Now () << " POS: x=" << pos.x << ", y=" << pos.y
      << ", z=" << pos.z << "; VEL:" << vel.x << ", y=" << vel.y
      << ", z=" << vel.z << std::endl;
}*/

// Example to use ns2 traces file in ns3
int main (int argc, char *argv[])
{
  std::string traceFile;
  std::string logFile;

  uint16_t numNodes;
  //uint8_t EmergencyCar = 1;
  //uint8_t RSUNodes = 100;

  //uint16_t numNodes2 = EmergencyCar + RSUNodes;  

  double duration;

  // Enable logging from the ns2 helper
  //LogComponentEnable ("Ns2MobilityHelper",LOG_LEVEL_DEBUG);

  // Parse command line attribute
  CommandLine cmd;
  cmd.AddValue ("traceFile", "Ns2 movement trace file", traceFile);
  cmd.AddValue ("numNodes", "Number of nodes", numNodes);
  cmd.AddValue ("duration", "Duration of Simulation", duration);
  cmd.AddValue ("logFile", "Log file", logFile);
  cmd.Parse (argc,argv);

  // Check command line arguments
  /*if (traceFile.empty () || numNodes <= 0 || duration <= 0 || logFile.empty ())
    {
      std::cout << "Usage of " << argv[0] << " :\n\n"
      "./waf --run \"ns2-mobility-trace"
      " --traceFile=src/mobility/examples/default.ns_movements"
      " --numNodes=2 --duration=100.0 --logFile=ns2-mob.log\" \n\n"
      "NOTE: ns2-traces-file could be an absolute or relative path. You could use the file default.ns_movements\n"
      "      included in the same directory of this example file.\n\n"
      "NOTE 2: Number of nodes present in the trace file must match with the command line argument and must\n"
      "        be a positive number. Note that you must know it before to be able to load it.\n\n"
      "NOTE 3: Duration must be a positive number. Note that you must know it before to be able to load it.\n\n";

      return 0;
    }*/

  // Create Ns2MobilityHelper with the specified trace log file as parameter
  Ns2MobilityHelper ns2 = Ns2MobilityHelper (traceFile);

  // open log file for output
  std::ofstream os;
  os.open (logFile.c_str ());

  /////////// Create all nodes/////////////////////////////////////////
   NodeContainer nodes;
   nodes.Create(numNodes);

   //NodeContainer carnodes;
   //carnodes.Create(numNodes);
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
  //NetDeviceContainer devices2;
  devices = wifi80211p.Install (wifiPhy, wifi80211pMac, nodes);
  //devices2 = wifi80211p.Install(wifiPhy, wifi80211pMac, carnodes);


   std::cout<<"Devices installed\n";
   


   ////////////////////////   MOBILITY  ///////////////////////////////////////////

  // Create mobility
  MobilityHelper mobility; 
  Ptr<ConstantVelocityMobilityModel> cvmm;
  Ptr<ConstantPositionMobilityModel> cpmm;
  
//変更------------------------------------------------------------------
  uint16_t emergencycar = 100;

  //緊急車両
  mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  mobility.Install(nodes.Get(emergencycar));
  cvmm = nodes.Get(emergencycar)->GetObject<ConstantVelocityMobilityModel> ();
  cvmm->SetPosition(Vector(0,0,0));
  cvmm->SetVelocity(Vector(12,0,0));

  

  


  //路側機
  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+emergencycar));
    cpmm = nodes.Get(i+emergencycar)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,0,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+10+emergencycar));
    cpmm = nodes.Get(i+10+emergencycar)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,100,0));
  }  

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+20+emergencycar));
    cpmm = nodes.Get(i+20+emergencycar)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,200,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+30+emergencycar));
    cpmm = nodes.Get(i+30+emergencycar)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,300,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+40+emergencycar));
    cpmm = nodes.Get(i+40+emergencycar)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,400,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+50+emergencycar));
    cpmm = nodes.Get(i+50+emergencycar)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,500,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+60+emergencycar));
    cpmm = nodes.Get(i+60+emergencycar)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,600,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+70+emergencycar));
    cpmm = nodes.Get(i+70+emergencycar)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,700,0));
  }

  for(uint8_t i=1;i<=10;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+80+emergencycar));
    cpmm = nodes.Get(i+80+emergencycar)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,800,0));
  }

  for(uint8_t i=1;i<=9;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+90+emergencycar));
    cpmm = nodes.Get(i+90+emergencycar)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,900,0));
  }

  
  //目的ノード
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(nodes.Get(100+emergencycar));
  cpmm = nodes.Get(100+emergencycar)->GetObject<ConstantPositionMobilityModel> ();
  cpmm->SetPosition(Vector(1000,900,0));



  std::cout<<"mobility set \n";

//----------------------------------------------------------------------

   ////////////////////////   INTERNET STACK /////////////////////////

   KitagawaHelper kitagawaProtocol;

   Ipv4ListRoutingHelper listrouting;
   listrouting.Add(kitagawaProtocol, 10);

   InternetStackHelper internet;
   internet.SetRoutingHelper(listrouting);
   internet.Install (nodes);
   //internet.Install (carnodes);
  
   Ipv4AddressHelper ipv4;
   NS_LOG_INFO ("Assign IP Addresses.");
   ipv4.SetBase ("10.1.0.0", "255.255.0.0");
   Ipv4InterfaceContainer interfaces;
   interfaces = ipv4.Assign (devices);
   //interfaces.Add(ipv4.Assign (devices2));
   std::cout<<"Internet Stack installed\n";







  ns2.Install (); // configure movements for each node, while reading trace file

  // Configure callback for logging
  //Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   //MakeBoundCallback (&CourseChange, &os));

  Simulator::Stop (Seconds (duration));
  AnimationInterface anim("animation2.xml");
  Simulator::Run ();
  Simulator::Destroy ();

  os.close (); // close log file
  return 0;
}
