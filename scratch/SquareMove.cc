#include "ns3/mobility-module.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/netanim-module.h"
#include <vector>

using namespace ns3;
 Ptr<Node> node;
 MobilityHelper mobility;

int main(){

  int numNodes = 13;

  NodeContainer nodes;
  nodes.Create(numNodes);
  
  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(nodes.Get(0));
  Ptr<ConstantPositionMobilityModel> cpmm;
  cpmm =  nodes.Get(0)->GetObject<ConstantPositionMobilityModel> ();
  cpmm->SetPosition(Vector(0,0,0));

  mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
  mobility.Install(nodes.Get(12));
  cpmm = nodes.Get(12)->GetObject<ConstantPositionMobilityModel> ();
  cpmm->SetPosition(Vector(300,300,0));
  
  for(uint8_t i=1;i<=3;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i));
    cpmm = nodes.Get(i)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,0,0));
  }
  for(uint8_t i=1;i<=3;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+3));
    cpmm = nodes.Get(i+3)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,100,0));
  }
  for(uint8_t i=1;i<=3;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+6));
    cpmm = nodes.Get(i+6)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,200,0));
  }
  for(uint8_t i=1;i<=2;i++){
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes.Get(i+9));
    cpmm = nodes.Get(i+9)->GetObject<ConstantPositionMobilityModel> ();
    cpmm->SetPosition(Vector(i*100,300,0));
  }
  
  
  AnimationInterface anim("animation6.xml");
  //anim.SetMobilityPollInterval(Seconds(0.1));
  Simulator::Stop(Seconds(5));
  Simulator::Run();
  Simulator::Destroy();
  return 0;
}
