// Network topology
    // //
    // //             pool   r    user
    // //             |    _    |
    // //             ====|_|====
    // //                router
    // //
    // // - Tracing of queues and packet receptions to file "simple-routing-ping6.tr"
    
    #include <fstream>
    #include "ns3/core-module.h"
    #include "ns3/internet-module.h"
    #include "ns3/csma-module.h"
	#include "ns3/applications-module.h"
	#include "vector"
	#include "iostream"
	#include "utility"
    //#include "ns3/internet-apps-module.h"
    #include "ns3/ipv6-static-routing-helper.h"
    
    #include "ns3/ipv6-routing-table-entry.h"
    
    using namespace ns3;
    
    NS_LOG_COMPONENT_DEFINE ("SimpleRoutingPing6Example");
    
    class StackHelper
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
    
    int main (int argc, char** argv)
    {
    #if 0 
      LogComponentEnable ("Ipv6L3Protocol", LOG_LEVEL_ALL);
      LogComponentEnable ("Icmpv6L4Protocol", LOG_LEVEL_ALL);
      LogComponentEnable ("Ipv6StaticRouting", LOG_LEVEL_ALL);
     LogComponentEnable ("Ipv6Interface", LOG_LEVEL_ALL);
     LogComponentEnable ("Ping6Application", LOG_LEVEL_ALL);
   #endif
   
     CommandLine cmd;
     cmd.Parse (argc, argv);
   
    StackHelper stackHelper;
    std::vector<std::pair<std::string,int> > vnfs;

   	std::pair<std::string,int> pa;

   	vnfs.push_back(std::make_pair("proxy",0));
   	vnfs.push_back(std::make_pair("firewall",0));
   	vnfs.push_back(std::make_pair("DHCP",0));
   	vnfs.push_back(std::make_pair("ids",0));
   	vnfs.push_back(std::make_pair("dns",0));
   	vnfs.push_back(std::make_pair("NAT",0));
   	vnfs.push_back(std::make_pair("load_balancer",0));
   	vnfs.push_back(std::make_pair("proxy",0));


     NS_LOG_INFO ("Create nodes.");
     Ptr<Node> pool = CreateObject<Node> ();
     Ptr<Node> r = CreateObject<Node> ();
     Ptr<Node> user = CreateObject<Node> ();
   	 Ptr<Node> cache = CreateObject<Node>();


     NodeContainer net1 (pool, r);
     NodeContainer net2 (r, user);
     NodeContainer all (pool, r, user);
     NS_LOG_INFO ("Create IPv6 Internet Stack");
     InternetStackHelper internetv6;
     internetv6.Install (all);
   
     NS_LOG_INFO ("Create channels.");
     CsmaHelper csma;
     csma.SetChannelAttribute ("DataRate", DataRateValue (10000000));
     csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
    NetDeviceContainer d1 = csma.Install (net1);
     NetDeviceContainer d2 = csma.Install (net2);
   
     NS_LOG_INFO ("Create networks and assign IPv6 Addresses.");
     Ipv6AddressHelper ipv6;
     ipv6.SetBase (Ipv6Address ("2001:1::"), Ipv6Prefix (64));
     Ipv6InterfaceContainer i1 = ipv6.Assign (d1);
     i1.SetForwarding (1, true);
     i1.SetDefaultRouteInAllNodes (1);
     ipv6.SetBase (Ipv6Address ("2001:2::"), Ipv6Prefix (64));
     Ipv6InterfaceContainer i2 = ipv6.Assign (d2);
    i2.SetForwarding (0, true);
     i2.SetDefaultRouteInAllNodes (0);
   
     stackHelper.PrintRoutingTable (pool);
   
     /* Create a Ping6 application to send ICMPv6 echo request from pool to user via r */
     uint32_t packetSize = 1024;
     uint32_t maxPacketCount = 5;
     Time interPacketInterval = Seconds (1.);
     Ping6Helper ping6;
   
     ping6.SetLocal (i1.GetAddress (0, 1));
     ping6.SetRemote (i2.GetAddress (1, 1)); 
   
     ping6.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
     ping6.SetAttribute ("Interval", TimeValue (interPacketInterval));
     ping6.SetAttribute ("PacketSize", UintegerValue (packetSize));
     ApplicationContainer apps = ping6.Install (net1.Get (0));
     apps.Start (Seconds (2.0));
     apps.Stop (Seconds (20.0));
   
     AsciiTraceHelper ascii;
     csma.EnableAsciiAll (ascii.CreateFileStream ("simple-routing-ping6.tr"));
     csma.EnablePcapAll ("simple-routing-ping6", true);
   
     NS_LOG_INFO ("Run Simulation.");
     Simulator::Run ();
     Simulator::Destroy ();
     NS_LOG_INFO ("Done.");
   }