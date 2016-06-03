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


    NS_LOG_COMPONENT_DEFINE ("UdpTrafficTest");



    int main(int argc, char  **argv)
    {
        /* code */
        #if 0
            LogComponentEnable ("UdpEchoClientApplication", LOG_LEVEL_ALL);
            LogComponentEnable ("UdpEchoServerApplication", LOG_LEVEL_ALL);
        #endif

            CommandLine cmd;
     cmd.Parse (argc, argv);

            NS_LOG_INFO ("Create nodes.");
     Ptr<Node> pool = CreateObject<Node> ();
     Ptr<Node> r = CreateObject<Node> ();
     Ptr<Node> user = CreateObject<Node> ();
     Ptr<Node> cache = CreateObject<Node>();

     Address serverAddress,clientAddress;
     NodeContainer net1 (user, r);
     NodeContainer net2 (r, pool);
     NodeContainer all (user, r, pool);
     NS_LOG_INFO ("Create IPv6 Internet Stack");
     InternetStackHelper internetv6;
     internetv6.Install (all);


             NS_LOG_INFO ("Create channels.");
     CsmaHelper csma;
     csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate("1Gb/s")));
     csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds (2)));
     csma.SetDeviceAttribute ("Mtu", UintegerValue (1400));

       NetDeviceContainer d1 = csma.Install (net1);
     NetDeviceContainer d2 = csma.Install (net2);

 NS_LOG_INFO ("Create networks and assign IPv6 Addresses.");
     Ipv6AddressHelper ipv6;
     ipv6.SetBase (Ipv6Address ("10:1::"), Ipv6Prefix (64));
     Ipv6InterfaceContainer i1 = ipv6.Assign (d1);
     clientAddress = Address(i1.GetAddress(0,1));

     i1.SetForwarding (1, true);
     i1.SetDefaultRouteInAllNodes (1);
     ipv6.SetBase (Ipv6Address ("10:2::"), Ipv6Prefix (64));
     Ipv6InterfaceContainer i2 = ipv6.Assign (d2);
     serverAddress = Address(i2.GetAddress(1,1));

     i2.SetForwarding (0, true);
     i2.SetDefaultRouteInAllNodes (0);
   


   uint16_t port = 4000;  //
   UdpServerHelper server (port);
  ApplicationContainer apps = server.Install (NodeContainer(user));
  apps.Start (Seconds (1.0));
  apps.Stop (Seconds (10.0));


   uint32_t packetSize = 1024;
  uint32_t maxPacketCount = 1;
  Time interPacketInterval = Seconds (1.);
  UdpEchoClientHelper client (serverAddress, port);
  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (packetSize));
  apps = client.Install (user);
  apps.Start (Seconds (2.0));
  apps.Stop (Seconds (10.0));


NS_LOG_INFO ("Run Simulation.");
  Simulator::Run ();
  Simulator::Destroy ();
  NS_LOG_INFO ("Done.");



        return 0;
    }