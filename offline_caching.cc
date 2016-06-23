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
  using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("OfflineCachingTest");



int main(int argc, char *argv[])
{
   /* code */
   CommandLine cmd;
   cmd.Parse(argc,argv);

   Ptr<Node> nA = CreateObject<Node> ();
    Ptr<Node> r = CreateObject<Node> ();
     Ptr<Node> nC = CreateObject<Node> ();

  Ptr<Node> ncache = CreateObject<Node> ();


   NodeContainer c= NodeContainer(nA,r,nC,ncache);
   InternetStackHelper internet;
   internet.Install(c);

   //p2p links

   NodeContainer nAr = NodeContainer(nA,r);
   NodeContainer rnC = NodeContainer(r,nC);
   NodeContainer rncache = NodeContainer(r,ncache);
   NodeContainer ncachenA = NodeContainer(ncache,nA);


   PointToPointHelper p2p;
   p2p.SetDeviceAttribute("DataRate",StringValue("1Gb/s"));
   p2p.SetChannelAttribute("Delay",StringValue("2ms"));

      PointToPointHelper p2p1;
   p2p1.SetDeviceAttribute("DataRate",StringValue("10Gb/s"));
   p2p1.SetChannelAttribute("Delay",StringValue("2ms"));

   NetDeviceContainer dAdr = p2p.Install(nAr);
   NetDeviceContainer drdC =  p2p.Install(rnC);
  NetDeviceContainer drdcache =  p2p.Install(rncache);
  NetDeviceContainer dcachedA =  p2p1.Install(ncachenA);



     Ptr<CsmaNetDevice> deviceA = CreateObject<CsmaNetDevice> ();
      deviceA->SetAddress (Mac48Address::Allocate ());
     nA->AddDevice (deviceA);

     Ptr<CsmaNetDevice> devicer = CreateObject<CsmaNetDevice> ();
      devicer->SetAddress (Mac48Address::Allocate ());
     r->AddDevice (devicer);

      Ptr<CsmaNetDevice> deviceC = CreateObject<CsmaNetDevice> ();
      deviceC->SetAddress (Mac48Address::Allocate ());
     nC->AddDevice (deviceC);

     Ptr<CsmaNetDevice> devicecache = CreateObject<CsmaNetDevice> ();
      devicecache->SetAddress (Mac48Address::Allocate ());
     ncache->AddDevice (devicecache);

    

     //Ip adresses

     Ipv4AddressHelper ipv4;
     ipv4.SetBase("10.1.1.0","255.255.255.252");
     Ipv4InterfaceContainer iAr = ipv4.Assign(dAdr);

    ipv4.SetBase("10.1.1.4","255.255.255.252");
     Ipv4InterfaceContainer irC = ipv4.Assign(drdC);    

     ipv4.SetBase("10.1.1.8","255.255.255.252");
     Ipv4InterfaceContainer ircache = ipv4.Assign(drdcache); 

     ipv4.SetBase("10.1.1.12","255.255.255.252");
     Ipv4InterfaceContainer icacheA = ipv4.Assign(dcachedA);  


     Ptr<Ipv4> ipv4A = nA->GetObject<Ipv4>();
     Ptr<Ipv4> ipv4r = r->GetObject<Ipv4>();
     Ptr<Ipv4> ipv4C = nC->GetObject<Ipv4>();
     Ptr<Ipv4> ipv4cache = ncache->GetObject<Ipv4>();

     int32_t ifIndexA = ipv4A->AddInterface(deviceA);
     int32_t ifIndexC = ipv4C->AddInterface(deviceC);
      int32_t ifIndexcache = ipv4cache->AddInterface(devicecache);



     //doubt in this


      Ipv4InterfaceAddress ifInAddrA = Ipv4InterfaceAddress (Ipv4Address ("172.16.1.1"), Ipv4Mask ("/32"));
      ipv4A->AddAddress (ifIndexA, ifInAddrA);
     ipv4A->SetMetric (ifIndexA, 1);
     ipv4A->SetUp (ifIndexA);
    
      Ipv4InterfaceAddress ifInAddrC = Ipv4InterfaceAddress (Ipv4Address ("192.168.1.1"), Ipv4Mask ("/32"));
      ipv4C->AddAddress (ifIndexC, ifInAddrC);
      ipv4C->SetMetric (ifIndexC, 1);
     ipv4C->SetUp (ifIndexC);


     Ipv4InterfaceAddress ifInAddrcache = Ipv4InterfaceAddress (Ipv4Address ("192.162.1.1"), Ipv4Mask ("/32"));
      ipv4cache->AddAddress (ifIndexcache, ifInAddrcache);
      ipv4cache->SetMetric (ifIndexcache, 1);
     ipv4cache->SetUp (ifIndexcache);








     Ipv4StaticRoutingHelper ipv4RoutingHelper;
     Ptr<Ipv4StaticRouting> staticRoutingA = ipv4RoutingHelper.GetStaticRouting(ipv4A);
     staticRoutingA->SetDefaultRoute(Ipv4Address("10.1.1.4"),1,0);

     Ptr<Ipv4StaticRouting> staticRoutingC = ipv4RoutingHelper.GetStaticRouting(ipv4C);
     staticRoutingC->SetDefaultRoute(Ipv4Address("10.1.1.4"),1,0);

     Ptr<Ipv4StaticRouting> staticRoutingr = ipv4RoutingHelper.GetStaticRouting(ipv4r);
     staticRoutingr->AddHostRouteTo(Ipv4Address("10.1.1.2"),Ipv4Address("10.1.1.2"),1,0);
     staticRoutingr->AddHostRouteTo(Ipv4Address("10.1.1.6"),Ipv4Address("10.1.1.6"),2,0);



     std::string destination = "10.1.1.1";
     //set destintaion based of vnfs in cache
     staticRoutingr->AddHostRouteTo(Ipv4Address(destination.c_str()),Ipv4Address(destination.c_str()),1,0);

     Ptr<Ipv4StaticRouting> staticRoutingcache = ipv4RoutingHelper.GetStaticRouting(ipv4cache);
     staticRoutingcache->AddHostRouteTo(Ipv4Address("10.1.1.0"),Ipv4Address("10.1.1.0"),1,0);


      // 210 bytes at a rate of 448 Kb/s
     uint16_t port = 9;   // Discard port (RFC 863)
     OnOffHelper onoff ("ns3::UdpSocketFactory", 
                        Address (InetSocketAddress (ifInAddrC.GetLocal (), port)));
     onoff.SetConstantRate (DataRate (6000));
     ApplicationContainer apps = onoff.Install (nA);
     apps.Start (Seconds (1.0));
     apps.Stop (Seconds (10.0));
     // Create a packet sink to receive these packets
     PacketSinkHelper sink ("ns3::UdpSocketFactory",
                            Address (InetSocketAddress (Ipv4Address::GetAny (), port)));
     apps = sink.Install (nC);
     apps.Start (Seconds (1.0));
     apps.Stop (Seconds (10.0));
   
     AsciiTraceHelper ascii;
     p2p.EnableAsciiAll (ascii.CreateFileStream ("static-routing-slash32.tr"));
     p2p.EnablePcapAll ("static-routing-slash32");
   
     Simulator::Run ();
    Simulator::Destroy ();


   return 0;
}