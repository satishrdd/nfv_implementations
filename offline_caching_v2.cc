#include <iostream>
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
NS_LOG_COMPONENT_DEFINE ("OfflineCachingTest");


   class StackHelper  								//to print the routing table
    {
    public:			
    
      inline void AddAddress (Ptr<Node>& n, uint32_t interface, Ipv6Address address)
      {
        Ptr<Ipv6> ipv6 = n->GetObject<Ipv6> ();
        ipv6->AddAddress (interface, address);
      }
    
      inline void PrintRoutingTable (Ptr<Node>& n)
      {
        Ptr<Ipv6StaticRouting> routing = 0;
        Ipv6StaticRoutingHelper routingHelper;
        Ptr<Ipv6> ipv6 = n->GetObject<Ipv6> ();
        uint32_t nbRoutes = 0;
        Ipv6RoutingTableEntry route;
    
        routing = routingHelper.GetStaticRouting (ipv6);
    
        std::cout << "Routing table of " << n << " : " << std::endl;
        std::cout << "Destination\t\t\t\t" << "Gateway\t\t\t\t\t" << "Interface\t" <<  "Prefix to use" << std::endl;
    
        nbRoutes = routing->GetNRoutes ();
        for (uint32_t i = 0; i < nbRoutes; i++)
          {
            route = routing->GetRoute (i);
            std::cout << route.GetDest () << "\t"
                      << route.GetGateway () << "\t"
                      << route.GetInterface () << "\t"
                      << route.GetPrefixToUse () << "\t"
                      << std::endl;
          }
      }
    };


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

//	StackHelper stackhelper;
	//create nodes

	Ptr<Node> nA = CreateObject<Node> ();
	Ptr<Node> r_1 = CreateObject<Node> ();
	Ptr<Node> r_2 = CreateObject<Node> ();
	Ptr<Node> nC = CreateObject<Node> ();
	Ptr<Node> ncache = CreateObject<Node>();

	CsmaHelper p2p;
   p2p.SetChannelAttribute("DataRate",StringValue("1Gbps"));
   p2p.SetChannelAttribute("Delay",StringValue("2ms"));

      CsmaHelper p2p1;
   p2p1.SetChannelAttribute("DataRate",StringValue("10Gbps"));
   p2p1.SetChannelAttribute("Delay",StringValue("2ms"));
   																						//c--->r_1-->cache-->r_2-->A

   NodeContainer c = NodeContainer(nA,r_1,r_2,nC,ncache);

   InternetStackHelper internet ;
   internet.Install(c);


   NodeContainer nCr_1  =  NodeContainer(nC,r_1);
   NodeContainer r_1ncache =  NodeContainer(r_1,ncache) ;
   NodeContainer r_2ncache= NodeContainer(r_2,ncache);
   NodeContainer r_2nA = NodeContainer(r_2,nA);



   NetDeviceContainer dAdr_2 = p2p1.Install(r_2nA);
   NetDeviceContainer dr_1dcache = p2p1.Install(r_1ncache);
   NetDeviceContainer dcachedr_2 = p2p1.Install(r_2ncache);
   NetDeviceContainer dr_1dC = p2p.Install(nCr_1);




   
   Ipv4AddressHelper ipv4;
   ipv4.SetBase("10.1.1.0","255.255.255.0");
   Ipv4InterfaceContainer iar_2;
   iar_2= ipv4.Assign(dAdr_2);

   ipv4.SetBase("10.1.2.0","255.255.255.0");
    Ipv4InterfaceContainer ir_1cache;
    ir_1cache = ipv4.Assign(dr_1dcache);

	ipv4.SetBase("10.1.3.0","255.255.255.0");
    Ipv4InterfaceContainer icacher_2;
    icacher_2= ipv4.Assign(dcachedr_2);

    ipv4.SetBase("10.1.4.0","255.255.255.0");
    Ipv4InterfaceContainer icr_1;
    icr_1= ipv4.Assign(dr_1dC);


    Ptr<Ipv4> ipv4A = nA->GetObject<Ipv4>();
     Ptr<Ipv4> ipv4r_1 = r_1->GetObject<Ipv4>();
     Ptr<Ipv4> ipv4r_2= r_2->GetObject<Ipv4>();
     Ptr<Ipv4> ipv4C = nC->GetObject<Ipv4>();
     Ptr<Ipv4> ipv4cache = ncache->GetObject<Ipv4>();  



     Ipv4InterfaceAddress iaddr = ipv4A->GetAddress(1,0);
     Ipv4Address addrA = iaddr.GetLocal();
      iaddr = ipv4C->GetAddress(1,0);
     Ipv4Address addrC = iaddr.GetLocal();





     //done till here

/*

      Ipv4InterfaceAddress iaddr = ipv4cache->GetAddress (1,0);
        Ipv4Address addrcache = iaddr.GetLocal (); 


          iaddr = ipv4A->GetAddress (1,0);
        Ipv4Address addrA = iaddr.GetLocal (); 
          iaddr = ipv4C->GetAddress (1,0);
        Ipv4Address addrC = iaddr.GetLocal (); 
          iaddr = ipv4r->GetAddress (3,0);
          //iaddr1 = ipv4r->GetAddress (2,0);
        Ipv4Address addrr = iaddr.GetLocal ();		//interface of rA
        iaddr = ipv4r->GetAddress (2,0); 
        Ipv4Address addrr1 = iaddr.GetLocal (); 		//interface of rcache
        iaddr = ipv4r->GetAddress (3,0);
         Ipv4Address addrr2 = iaddr.GetLocal (); 		//interface of rC


        std::cout<<irc.GetAddress(0)<<" "<<ircache.GetAddress(0)<<" "<<iar.GetAddress(1)<<" "<<irc.GetAddress(1)<<" "<<iar.GetAddress(0)<<" "<<ircache.GetAddress(1)<<std::endl;



Ipv4StaticRoutingHelper ipv4RoutingHelper;
     

     Ptr<Ipv4StaticRouting> staticRoutingC = ipv4RoutingHelper.GetStaticRouting(ipv4C);
     staticRoutingC->AddHostRouteTo(addrA,addrr2,1);
     staticRoutingC->SetDefaultRoute(addrr2,1);

     Ptr<Ipv4StaticRouting> staticRoutingr = ipv4RoutingHelper.GetStaticRouting(ipv4r);
     staticRoutingr->AddHostRouteTo(addrA,addrA,1);
     staticRoutingr->SetDefaultRoute(addrA,1);
    // staticRoutingr->SetDefaultRoute(addrC,3);

     Ptr<Ipv4StaticRouting> staticRoutingA = ipv4RoutingHelper.GetStaticRouting(ipv4A);
     //staticRoutingA->SetDefaultRoute(addrr,1);
  


  std::cout<<"A"<<addrA<<" "<<"C"<<addrC<<" router"<<addrr<<" cache"<<addrcache<<std::endl;





  stackhelper.PrintRoutingTable(nC);

        
    uint16_t port =9;
        

    BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (addrA, port));


    source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer sourceApps = source.Install (rnC.Get (1));
  sourceApps.Start (Seconds (0.0));
  sourceApps.Stop (Seconds (100.0));

  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = sink.Install (nAr.Get(0));
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (100.0));


//

//
// Now, do the actual simulation.
//

  FlowMonitorHelper flowmon;
  Ptr<FlowMonitor> monitor = flowmon.InstallAll();


  NS_LOG_INFO ("Run Simulation.");
  Simulator::Stop (Seconds (100.0));
  Simulator::Run ();

    monitor->CheckForLostPackets ();

    Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
  std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
  for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
	  Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      if ((t.sourceAddress==addrr && t.destinationAddress == addrA))
      {
          std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
          std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
      	  std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
      }
     }



  monitor->SerializeToXmlFile("direct_1.flowmon", true, true);

  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  Ptr<PacketSink> sink1 = DynamicCast<PacketSink> (sinkApps.Get (0));
  std::cout << "Total Bytes Received: " << sink1->GetTotalRx () << std::endl;*/
	return 0;
}