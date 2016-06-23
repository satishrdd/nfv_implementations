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
NS_LOG_COMPONENT_DEFINE ("OfflineCachingTest");
#define cachesize 4;
int t;
int hitrate=0;





int main(int argc, char  *argv[])
{
  /* code */
  uint32_t maxBytes = 0;
  bool tracing = false;
  CommandLine cmd;
  cmd.AddValue ("tracing", "Flag to enable/disable tracing", tracing);
    cmd.AddValue ("maxBytes",
                "Total number of bytes for application to send", maxBytes);
  cmd.Parse(argc,argv);


  std::vector<std::string> cache;
  std::vector<std::string> cache2;
  std::vector<std::string> v;
  std::map<std::string, int> clonetime;
  std::map<std::string, int> location;
  v.clear();
  v.push_back("v1");
  v.push_back("v2");
  v.push_back("v3");
  v.push_back("v4");
  v.push_back("v5");
  v.push_back("v6");
  v.push_back("v7");
  v.push_back("v8");
  v.push_back("vf1");
  v.push_back("v10");

  for(int i=0;i<v.size();i++){
    clonetime[v[i]] = i+1;          //created a map to clone times
  }


  int ar[10];
  for(int i=0;i<10;i++){
    ar[i] =i;
  }

  srand(time(NULL));
  for(int i=0;i<10;i++){

    int j = rand()%10;
    if(i!=j){
      swap(v[ar[i]],v[ar[j]]);
    }
  }
  
  cache.clear();
  
  cache.push_back(v[0]);
  cache.push_back(v[1]);
  cache.push_back(v[2]);
  cache.push_back(v[3]);

  std::cout<<cache[0]<<" "<<cache[1]<<" "<<cache[2]<<" "<<cache[3]<<std::endl;
  t=0;
  int no;
  int firewallflag=0;

  std::cout<<"Enter vnfs needed should ne less than 10\n";
  std::cin>>no;
  std::string req[no];
  std::string user[no];
  double t[no];
  double simulationtime=0,maxalivetime=0;
  std::cout<<"Enter vnf name and alive times of each and user 1 or 2 requesting file\n";
  for(int i=0;i<no;i++){
    std::cin>>req[i];
    std::cin>>t[i];
    std::cin>>user[i];
    if(req[i]=="vf2"||req[i]=="vf1"){
      firewallflag=1;
    }
    if(t[i]>maxalivetime){
      maxalivetime =t[i];
    }
    simulationtime += t[i];
  }



    for(int i=0;i<no;i++){
      for(int j=0;j<4;j++)
        if(req[i]==cache[j]){
          hitrate++;
          
        }
    }

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
          
    Ipv4Address addrr = iaddr.GetLocal ();    //interface of rA
    iaddr = ipv4r->GetAddress (2,0); 
    Ipv4Address addrr1 = iaddr.GetLocal ();     //interface of rcache
    iaddr = ipv4r->GetAddress (3,0);
    Ipv4Address addrr2 = iaddr.GetLocal ();     //interface of rC


    std::cout<<irc.GetAddress(0)<<" "<<ircache.GetAddress(0)<<" "<<iar.GetAddress(1)<<" "<<irc.GetAddress(1)<<" "<<iar.GetAddress(0)<<" "<<ircache.GetAddress(1)<<std::endl;



  if(firewallflag==0){
          //only 3 cases;
    int poolflag=0,cacheflag=0;
    for(int i=0;i<no;i++){
      for(int j=0;j<4;j++)
        if(req[i]==cache[j]){
          cacheflag=1;
          location[req[i]]=2;
          
        }else{
          poolflag=1;
          location[req[i]]=3;
        }
    }

  

      Ipv4StaticRoutingHelper ipv4RoutingHelper;
     

      Ptr<Ipv4StaticRouting> staticRoutingC = ipv4RoutingHelper.GetStaticRouting(ipv4C);
      staticRoutingC->AddHostRouteTo(addrA,addrr2,1);
      staticRoutingC->SetDefaultRoute(addrr2,1);

      Ptr<Ipv4StaticRouting> staticRoutingr = ipv4RoutingHelper.GetStaticRouting(ipv4r);
      staticRoutingr->AddHostRouteTo(addrA,addrA,1);
      staticRoutingr->SetDefaultRoute(addrA,1);
      // staticRoutingr->SetDefaultRoute(addrC,3);

        Ptr<Ipv4StaticRouting> staticRoutingcache = ipv4RoutingHelper.GetStaticRouting(ipv4cache);
      staticRoutingcache->AddHostRouteTo(addrA,addrr1,1);
      //staticRoutingA->SetDefaultRoute(addrr,1);

    for(int i=0;i<no;i++){
      uint16_t port =i+9;
          BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (addrA, port));
      source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
        ApplicationContainer sourceApps = source.Install (c.Get(location[req[i]]));
        sourceApps.Start (Seconds (0.0));
        sourceApps.Stop (Seconds (t[i]));
      PacketSinkHelper sink ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port));
        ApplicationContainer sinkApps = sink.Install (c.Get(0));
        sinkApps.Start (Seconds (0.0));
        sinkApps.Stop (Seconds (t[i]));
      }
    FlowMonitorHelper flowmon;
      Ptr<FlowMonitor> monitor = flowmon.InstallAll();


        Simulator::Stop (Seconds (maxalivetime));
      Simulator::Run ();

    monitor->CheckForLostPackets ();

      Ptr<Ipv4FlowClassifier> classifier = DynamicCast<Ipv4FlowClassifier> (flowmon.GetClassifier ());
      std::map<FlowId, FlowMonitor::FlowStats> stats = monitor->GetFlowStats ();
      for (std::map<FlowId, FlowMonitor::FlowStats>::const_iterator i = stats.begin (); i != stats.end (); ++i)
      {
        Ipv4FlowClassifier::FiveTuple t = classifier->FindFlow (i->first);
          if ((t.sourceAddress==addrC && t.destinationAddress == addrA))
        {
              std::cout << "Flow " << i->first  << " (" << t.sourceAddress << " -> " << t.destinationAddress << ")\n";
              std::cout << "  Tx Bytes:   " << i->second.txBytes << "\n";
              std::cout << "  Rx Bytes:   " << i->second.rxBytes << "\n";
              std::cout << "  Throughput: " << i->second.rxBytes * 8.0 / (i->second.timeLastRxPacket.GetSeconds() - i->second.timeFirstTxPacket.GetSeconds())/1024/1024  << " Mbps\n";
        }
      }





      monitor->SerializeToXmlFile("offline.flowmon", true, true);

      Simulator::Destroy ();

  }
  else{                 //4 cases
      if(cache[0]=="vf1"||cache[1]=="vf1"||cache[2]=="vf1"||cache[3]=="vf1")
      {
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
  
          for(int i=0;i<no;i++){
            if(req[i]!="vf1"&&req[i]!="vf2"){
            uint16_t port =9+i,port1=45+i;

                BulkSendHelper source ("ns3::TcpSocketFactory",InetSocketAddress (addrcache, port));
                        source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
              ApplicationContainer sourceApps = source.Install (c.Get(location[req[i]]));
              sourceApps.Start (Seconds (0.0));
              sourceApps.Stop (Seconds (t[i]/2));

              PacketSinkHelper sink ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port));
              ApplicationContainer sinkApps = sink.Install (ncache);
              sinkApps.Start (Seconds (0.0));
              sinkApps.Stop (Seconds (t[i]/2));

            BulkSendHelper source1 ("ns3::TcpSocketFactory", InetSocketAddress (addrA, port1));
            source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
              ApplicationContainer sourceApps1 = source1.Install (ncache);
              sourceApps1.Start (Seconds (t[i]/2+1));
              sourceApps1.Stop (Seconds (t[i]));

              PacketSinkHelper sink1 ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port1));
              ApplicationContainer sinkApps1 = sink1.Install (nA);  
              sinkApps1.Start (Seconds (t[i]/2+1));
              sinkApps1.Stop (Seconds (t[i]));
          }
        }

          FlowMonitorHelper flowmon;
          Ptr<FlowMonitor> monitor = flowmon.InstallAll();

          Simulator::Stop (Seconds (maxalivetime));
          Simulator::Run ();
          monitor->CheckForLostPackets ();
          monitor->SerializeToXmlFile("offline.flowmon", true, true);

      Simulator::Destroy ();

      }
      else{
        Ipv4StaticRoutingHelper ipv4RoutingHelper;
        Ptr<Ipv4StaticRouting> staticRoutingC = ipv4RoutingHelper.GetStaticRouting(ipv4C);
          staticRoutingC->AddHostRouteTo(addrA,addrr2,1);
     
          //staticRoutingC->SetDefaultRoute(addrr2,1);

          Ptr<Ipv4StaticRouting> staticRoutingr = ipv4RoutingHelper.GetStaticRouting(ipv4r);
          staticRoutingr->AddHostRouteTo(addrA,addrA,1);
          staticRoutingr->AddHostRouteTo(addrC,addrC,3);
          staticRoutingr->AddHostRouteTo(addrcache,addrcache,2);
          // staticRoutingr->SetDefaultRoute(addrA,1);
          // staticRoutingr->SetDefaultRoute(addrC,3);

          Ptr<Ipv4StaticRouting> staticRoutingcache = ipv4RoutingHelper.GetStaticRouting(ipv4cache);
          //staticRoutingA->SetDefaultRoute(addrr,1);
          staticRoutingcache->AddHostRouteTo(addrC,addrr1,1);
          for(int i=0;i<no;i++){
            if(req[i]!="vf1"&&req[i]!="vf2"){
            uint16_t port =9+i,port1=45+i;
                BulkSendHelper source ("ns3::TcpSocketFactory", InetSocketAddress (addrC, port));
            source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
              ApplicationContainer sourceApps = source.Install (c.Get(location[req[i]]));
              sourceApps.Start (Seconds (0.0));
              sourceApps.Stop (Seconds (t[i]/2));

              PacketSinkHelper sink ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port));
              ApplicationContainer sinkApps = sink.Install (nC);
              sinkApps.Start (Seconds (0.0));
              sinkApps.Stop (Seconds (t[i]/2));

            BulkSendHelper source1 ("ns3::TcpSocketFactory",InetSocketAddress (addrA, port1));
            source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
              ApplicationContainer sourceApps1 = source1.Install (nC);
              sourceApps1.Start (Seconds (t[i]/2+1));
              sourceApps1.Stop (Seconds (t[i]));

              PacketSinkHelper sink1 ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port1));
              ApplicationContainer sinkApps1 = sink1.Install (nA);
              sinkApps1.Start (Seconds (t[i]/2+1));
              sinkApps1.Stop (Seconds (t[i]));
          }

        } 



        FlowMonitorHelper flowmon;
        Ptr<FlowMonitor> monitor = flowmon.InstallAll();


        Simulator::Stop (Seconds (maxalivetime));
        Simulator::Run ();
        monitor->CheckForLostPackets ();
        monitor->SerializeToXmlFile("offline.flowmon", true, true);

      Simulator::Destroy ();



      }}

std::cout<<(double)hitrate/(double)no<<std::endl;
  return 0;
}