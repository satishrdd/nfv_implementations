#include<vector>
#include<algorithm>
#include<map>
#include <fstream>
#include <string>
#include <cassert>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/csma-module.h"
#include "ns3/applications-module.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/flow-monitor-module.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("TopologyTest");
#define cachesize 4;



int main(int argc, char *argv[])
{
	/* code */
	 uint32_t maxBytes = 0;
  bool tracing = false;
  CommandLine cmd;
  cmd.AddValue ("tracing", "Flag to enable/disable tracing", tracing);
    cmd.AddValue ("maxBytes",
                "Total number of bytes for application to send", maxBytes);
  cmd.Parse(argc,argv);

  Ptr<Node> nu1 = CreateObject<Node> ();
  Ptr<Node> nu2 = CreateObject<Node> ();
  Ptr<Node> r = CreateObject<Node> ();
  Ptr<Node> r1 = CreateObject<Node> ();
  Ptr<Node> r2 = CreateObject<Node> ();
  Ptr<Node> nC = CreateObject<Node> ();
  Ptr<Node> ncache1 = CreateObject<Node>();
  Ptr<Node> ncache2 = CreateObject<Node>();

  CsmaHelper p2p;
    p2p.SetChannelAttribute("DataRate",StringValue("1Gbps"));
    p2p.SetChannelAttribute("Delay",StringValue("2ms"));

    CsmaHelper p2p1;
    p2p1.SetChannelAttribute("DataRate",StringValue("10Gbps"));
    p2p1.SetChannelAttribute("Delay",StringValue("2ms"));


    NodeContainer c1 = NodeContainer(nu1,nu2,r,r1,r2);
    NodeContainer c2 =NodeContainer(nC,ncache1,ncache2);

    InternetStackHelper internet ;
    internet.Install(c1);
    internet.Install(c2);

    NodeContainer rnC = NodeContainer(r,nC);
    NodeContainer rr1 = NodeContainer(r,r1);
    NodeContainer rr2 = NodeContainer(r,r2);
    NodeContainer r1ncache1 = NodeContainer(r1,ncache1);
    NodeContainer r2ncache2 = NodeContainer(r2,ncache2);
    NodeContainer r1nu1 = NodeContainer(r1,nu1);
    NodeContainer r2nu2  = NodeContainer(r2,nu2);

    NetDeviceContainer drdc = p2p.Install(rnC);
    NetDeviceContainer drdr1 = p2p.Install(rr1);
    NetDeviceContainer drdr2 = p2p.Install(rr2);
    NetDeviceContainer dr1dcache1 = p2p1.Install(r1ncache1);
    NetDeviceContainer dr2dcache2 = p2p1.Install(r2ncache2);
    NetDeviceContainer dr1du1 = p2p1.Install(r1nu1);
    NetDeviceContainer dr2du2 = p2p.Install(r2nu2);


    Ipv4AddressHelper ipv4;

    ipv4.SetBase("10.1.1.0","255.255.255.0");
    Ipv4InterfaceContainer irc;
    irc= ipv4.Assign(drdc);

    ipv4.SetBase("10.1.2.0","255.255.255.0");
    Ipv4InterfaceContainer irr1;
    irr1= ipv4.Assign(drdr1);

    ipv4.SetBase("10.1.3.0","255.255.255.0");
    Ipv4InterfaceContainer irr2;
    irr2= ipv4.Assign(drdr2);

    ipv4.SetBase("10.1.4.0","255.255.255.0");
    Ipv4InterfaceContainer ir1cache1;
    ir1cache1= ipv4.Assign(dr1dcache1);

    ipv4.SetBase("10.1.5.0","255.255.255.0");
    Ipv4InterfaceContainer ir2cache2;
    ir2cache2= ipv4.Assign(dr2dcache2);

    ipv4.SetBase("10.1.6.0","255.255.255.0");
    Ipv4InterfaceContainer ir1u1;
    ir1u1= ipv4.Assign(dr1du1);


    ipv4.SetBase("10.1.7.0","255.255.255.0");
    Ipv4InterfaceContainer ir2u2;
    ir2u2= ipv4.Assign(dr2du2);


     Ptr<Ipv4> ipv4c = nC->GetObject<Ipv4>();
    Ptr<Ipv4> ipv4r = r->GetObject<Ipv4>();
    Ptr<Ipv4> ipv4r1 = r1->GetObject<Ipv4>();
    Ptr<Ipv4> ipv4r2 = r2->GetObject<Ipv4>();
    Ptr<Ipv4> ipv4u1 = nu1->GetObject<Ipv4>();
     Ptr<Ipv4> ipv4u2 = nu2->GetObject<Ipv4>();
    Ptr<Ipv4> ipv4cache1 = ncache1->GetObject<Ipv4>(); 
    Ptr<Ipv4> ipv4cache2 = ncache2->GetObject<Ipv4>(); 



    Ipv4InterfaceAddress iaddr = ipv4c->GetAddress(1,0);
    Ipv4Address addrc = iaddr.GetLocal();

     iaddr = ipv4cache1->GetAddress(1,0);
    Ipv4Address addrcache1 = iaddr.GetLocal();

     iaddr = ipv4cache2->GetAddress(1,0);
    Ipv4Address addrcache2 = iaddr.GetLocal();

     iaddr = ipv4u1->GetAddress(1,0);
    Ipv4Address addru1 = iaddr.GetLocal();

    iaddr = ipv4u2->GetAddress(1,0);
    Ipv4Address addru2 = iaddr.GetLocal();

    iaddr = ipv4r->GetAddress(1,0);
    Ipv4Address addrr_1 = iaddr.GetLocal();			//interface with c

    iaddr = ipv4r->GetAddress(2,0);
    Ipv4Address addrr_2 = iaddr.GetLocal();			//interface with r1

    iaddr = ipv4r->GetAddress(3,0);
    Ipv4Address addrr_3 = iaddr.GetLocal();			//interface with r2


    iaddr = ipv4r1->GetAddress(1,0);
    Ipv4Address addrr1_1 = iaddr.GetLocal();		//interface with r

	iaddr = ipv4r1->GetAddress(2,0);
    Ipv4Address addrr1_2 = iaddr.GetLocal();    	//interface with cache1

    iaddr = ipv4r1->GetAddress(3,0);
    Ipv4Address addrr1_3 = iaddr.GetLocal();    	//interface with u1

    iaddr = ipv4r2->GetAddress(1,0);
    Ipv4Address addrr2_1 = iaddr.GetLocal();		//interface with r

	iaddr = ipv4r2->GetAddress(2,0);
    Ipv4Address addrr2_2 = iaddr.GetLocal();    	//interface with cache2

    iaddr = ipv4r2->GetAddress(3,0);
    Ipv4Address addrr2_3 = iaddr.GetLocal(); 	//interface with u2

   Ipv4StaticRoutingHelper ipv4RoutingHelper;
    Ptr<Ipv4StaticRouting> staticRoutingC = ipv4RoutingHelper.GetStaticRouting(ipv4c);
    staticRoutingC->AddHostRouteTo(addru1,addrr_1,1);
    staticRoutingC->AddHostRouteTo(addru2,addrr_1,1);
    staticRoutingC->AddHostRouteTo(addrcache1,addrr_1,1);
    staticRoutingC->AddHostRouteTo(addrcache2,addrr_1,1);

     Ptr<Ipv4StaticRouting> staticRoutingr = ipv4RoutingHelper.GetStaticRouting(ipv4r);
     staticRoutingr->AddHostRouteTo(addru1,addrr1_1,2);
     staticRoutingr->AddHostRouteTo(addru2,addrr2_1,3);
     staticRoutingr->AddHostRouteTo(addrcache1,addrr1_1,2);
     staticRoutingr->AddHostRouteTo(addrcache2,addrr2_1,3);
     staticRoutingr->AddHostRouteTo(addrc,addrc,1);

      Ptr<Ipv4StaticRouting> staticRoutingr1 = ipv4RoutingHelper.GetStaticRouting(ipv4r1);
      staticRoutingr1->AddHostRouteTo(addru1,addru1,3);
      staticRoutingr1->AddHostRouteTo(addrcache1,addrcache1,2);
      staticRoutingr1->AddHostRouteTo(addrc,addrr1_1,1);
       
      Ptr<Ipv4StaticRouting> staticRoutingr2 = ipv4RoutingHelper.GetStaticRouting(ipv4r2);
      staticRoutingr2->AddHostRouteTo(addru2,addru2,3);
      staticRoutingr2->AddHostRouteTo(addrcache2,addrcache2,2);
      staticRoutingr2->AddHostRouteTo(addrc,addrr2_1,1);
    

       Ptr<Ipv4StaticRouting> staticRoutingrcache1 = ipv4RoutingHelper.GetStaticRouting(ipv4cache1);
       staticRoutingrcache1->AddHostRouteTo(addru1,addrr1_2,1);
       staticRoutingrcache1->AddHostRouteTo(addrc,addrr1_2,1);

        Ptr<Ipv4StaticRouting> staticRoutingrcache2 = ipv4RoutingHelper.GetStaticRouting(ipv4cache2);
       staticRoutingrcache2->AddHostRouteTo(addru2,addrr2_2,1);
       staticRoutingrcache2->AddHostRouteTo(addrc,addrr2_2,1);
   Simulator::Stop (Seconds(100.0));
      Simulator::Run ();

	return 0;
}