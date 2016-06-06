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

	//StackHelper stackhelper;
	//create nodes

	Ptr<Node> nA = CreateObject<Node> ();
	Ptr<Node> r = CreateObject<Node> ();
	Ptr<Node> nC = CreateObject<Node> ();
	Ptr<Node> ncache = CreateObject<Node>();

	CsmaHelper p2p;
   p2p.SetChannelAttribute("DataRate",StringValue("1Gbps"));
   p2p.SetChannelAttribute("Delay",StringValue("2ms"));

      CsmaHelper p2p1;
   p2p1.SetChannelAttribute("DataRate",StringValue("10Gbps"));
   p2p1.SetChannelAttribute("Delay",StringValue("2ms"));


   NodeContainer c = NodeContainer(nA,r,nC,ncache);

   InternetStackHelper internet ;
   internet.Install(c);


   NodeContainer nAr  =  NodeContainer(nA,r);
   NodeContainer rnC =  NodeContainer(r,nC) ;
   NodeContainer rncache= NodeContainer(r,ncache);



   NetDeviceContainer dAdr = p2p1.Install(nAr);
   NetDeviceContainer drcache = p2p1.Install(rncache);
   NetDeviceContainer drdC = p2p.Install(rnC);




   
   Ipv4AddressHelper ipv4;
   ipv4.SetBase("10.1.1.0","255.255.255.0");
   Ipv4InterfaceContainer iar;
   iar= ipv4.Assign(dAdr);

   ipv4.SetBase("10.1.2.0","255.255.255.0");
    Ipv4InterfaceContainer ircache;
    ircache = ipv4.Assign(drcache);

	ipv4.SetBase("10.1.3.0","255.255.255.0");
    Ipv4InterfaceContainer irc;
    irc = ipv4.Assign(drdC);


    Ptr<Ipv4> ipv4A = nA->GetObject<Ipv4>();
     Ptr<Ipv4> ipv4r = r->GetObject<Ipv4>();
     Ptr<Ipv4> ipv4C = nC->GetObject<Ipv4>();
     Ptr<Ipv4> ipv4cache = ncache->GetObject<Ipv4>();  

      Ipv4InterfaceAddress iaddr = ipv4cache->GetAddress (1,0);
        Ipv4Address addrcache = iaddr.GetLocal (); 


          iaddr = ipv4A->GetAddress (1,0);
        Ipv4Address addrA = iaddr.GetLocal (); 
          iaddr = ipv4C->GetAddress (1,0);
        Ipv4Address addrC = iaddr.GetLocal (); 
          iaddr = ipv4r->GetAddress (1,0);
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
     staticRoutingC->AddHostRouteTo(addrcache,addrr2,1);
     staticRoutingC->SetDefaultRoute(addrr2,1);

     Ptr<Ipv4StaticRouting> staticRoutingr = ipv4RoutingHelper.GetStaticRouting(ipv4r);
     staticRoutingr->AddHostRouteTo(addrA,addrA,1);
     staticRoutingr->AddHostRouteTo(addrcache,addrcache,2);
     staticRoutingr->SetDefaultRoute(addrA,1);
    // staticRoutingr->SetDefaultRoute(addrC,3);

     Ptr<Ipv4StaticRouting> staticRoutingcache = ipv4RoutingHelper.GetStaticRouting(ipv4cache);
     //staticRoutingA->SetDefaultRoute(addrr,1);
     staticRoutingcache->AddHostRouteTo(addrA,addrr1,1);
  
uint16_t port =9,port1=10;
        

    BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (addrcache, port));


    source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer sourceApps = source.Install (nC);
  sourceApps.Start (Seconds (0.0));
  sourceApps.Stop (Seconds (50.0));

  PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
  ApplicationContainer sinkApps = sink.Install (ncache);
  sinkApps.Start (Seconds (0.0));
  sinkApps.Stop (Seconds (50.0));





    BulkSendHelper source1 ("ns3::TcpSocketFactory",
                         InetSocketAddress (addrA, port1));


    source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
  ApplicationContainer sourceApps1 = source1.Install (ncache);
  sourceApps1.Start (Seconds (51.0));
  sourceApps1.Stop (Seconds (100.0));

  PacketSinkHelper sink1 ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port1));
   ApplicationContainer sinkApps1 = sink1.Install (nA);
  sinkApps1.Start (Seconds (51.0));
  sinkApps1.Stop (Seconds (100.0));






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
      if ((t.sourceAddress==addrC && t.destinationAddress == addrcache))
      {
          std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
          std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
          std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
      }
     }

 for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
    {
    Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
      if ((t.sourceAddress==addrcache && t.destinationAddress == addrA))
      {
          std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
          std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
          std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
          std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
      }
     }


  monitor->SerializeToXmlFile("direct.flowmon", true, true);

  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");

  Ptr<PacketSink> sink2 = DynamicCast<PacketSink> (sinkApps1.Get (0));
  std::cout << "Total Bytes Received: " << sink2->GetTotalRx () << std::endl;
  return 0;
}