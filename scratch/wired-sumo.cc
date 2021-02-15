/*
 *
 * Usage of ns2-mobility-trace:
 *
 *  ./waf --run "ns2-mobility-trace \
 *        --traceFile=src/mobility/examples/default.ns_movements
 *        --nodeNum=2  --duration=100.0 --logFile=ns2-mobility-trace.log"
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
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/kitagawa-module.h"
#include "ns3/applications-module.h"
#include "ns3/ns2-mobility-helper.h"
#include "ns3/netanim-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"


#include "ns3/ocb-wifi-mac.h"
#include "ns3/wifi-80211p-helper.h"
#include "ns3/wave-mac-helper.h"

NS_LOG_COMPONENT_DEFINE ("KitagawaProtocol");

using namespace ns3;
Ptr<ConstantPositionMobilityModel> cpmm;
Ptr<ConstantVelocityMobilityModel> cvmm;


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
 
  uint16_t numNodes; //sumo
  uint16_t EmergencyNode = 1;
  //uint16_t CarNodes = numNodes;
  uint16_t csma_RSUNodes = 15;
  uint16_t adjustnodes = 2;
  
  double duration; //コマンドプロンプトで打ち込む

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
      " --nodeNum=2 --duration=100.0 --logFile=ns2-mob.log\" \n\n"
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




  //////// Create all nodes /////////////////////////////////////////
  NodeContainer carnodes;
  carnodes.Create (numNodes);
  std::cout<<"CarNodes created\n";
  
  NodeContainer emergencynode;
  emergencynode.Create (EmergencyNode);
  

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
  car_wireless_devices.Add(wifi80211p.Install(wifiPhy, wifi80211pMac,emergencynode));


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

///////////// Mobility /////////////////////

     // Create mobility
  MobilityHelper mobility;
  Ptr<ConstantVelocityMobilityModel> cvmm;
  Ptr<ConstantPositionMobilityModel> cpmm;
 
 
  //緊急車両
  mobility.SetMobilityModel("ns3::ConstantVelocityMobilityModel");
  mobility.Install(emergencynode.Get(0));
  cvmm = emergencynode.Get(0) -> GetObject<ConstantVelocityMobilityModel> ();
  cvmm->SetPosition(Vector(0,0,0));
  cvmm->SetVelocity(Vector(12,0,0));


  //RSU
  //消防署ノード
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(csma_rsunodes.Get(0));
  cpmm = csma_rsunodes.Get(0)->GetObject<ConstantPositionMobilityModel> ();
  cpmm->SetPosition(Vector(2000,0,0));


  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(ApNode.Get(0));
  cpmm = ApNode.Get(0)->GetObject<ConstantPositionMobilityModel> ();
  cpmm->SetPosition(Vector(100,0,0));

  for(uint16_t i=1;i<=3;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(csma_rsunodes.Get(i));
    cpmm = csma_rsunodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*300+100,0,0));
  }

  for(uint16_t i=4;i<=7;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(csma_rsunodes.Get(i));
    cpmm = csma_rsunodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector((i-4)*300+100,300,0));
  }

  for(uint16_t i=8;i<=11;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(csma_rsunodes.Get(i));
    cpmm = csma_rsunodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector((i-8)*300+100,600,0));
  }

  for(uint16_t i=12;i<=14;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(csma_rsunodes.Get(i));
    cpmm = csma_rsunodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector((i-12)*300+100,900,0));
  }

  //目的ノード
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(csma_rsunodes.Get(15));
  cpmm = csma_rsunodes.Get(15)->GetObject<ConstantPositionMobilityModel> ();
  cpmm->SetPosition(Vector(1000,900,0));


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


  ////////Internet Stack ///////////////////////////////////////////
   KitagawaHelper kitagawaProtocol;

   Ipv4ListRoutingHelper listrouting;
   listrouting.Add(kitagawaProtocol, 10);

   InternetStackHelper stack;
   stack.SetRoutingHelper(listrouting);
   stack.Install (carnodes);
   stack.Install (emergencynode);
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



  ns2.Install (); // configure movements for each node, while reading trace file

  // Configure callback for logging
  //Config::Connect ("/NodeList/*/$ns3::MobilityModel/CourseChange",
                   //MakeBoundCallback (&CourseChange, &os));
  AnimationInterface anim("animation.xml");

  Simulator::Stop (Seconds (duration));

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

  

  os.close (); // close log file
  return 0;
}
