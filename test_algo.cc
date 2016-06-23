#include <iostream>
 #include <fstream>
 #include <string>
 #include <cassert>
#include <vector>

 #include "ns3/core-module.h"
 #include "ns3/network-module.h"
  #include "ns3/internet-module.h"
 #include "ns3/point-to-point-module.h"
 #include "ns3/csma-module.h"
 #include "ns3/applications-module.h"
 #include "ns3/ipv4-static-routing-helper.h"
#include "ns3/packet-sink.h"
#include "ns3/flow-monitor-module.h"
#include <stack>
#include <queue>
#define cachesize 4

#define prweight 7
#define cloneweight 3



double minrank=0; 
  using namespace ns3;


std::vector<std::string> cache;
std::vector<std::string> v;
std::map<std::string, int> clonetime;
std::map<std::string, int> priority;
std::map<std::string, int> location;
std::map<std::string, double> rank;



void update(std::string s){
  int flag=1;
  int pos=0;
  int vnfflag=0;
  

  for(int i=0;i<v.size();i++){
    if(v[i]==s){
      vnfflag=1;
      break;
    }
  }

  if(vnfflag==0){
    std::cout<<"This vnf is not available";
    //break;
  }

  
  for(int i=0;i<cache.size();i++){
    //cout<<"re"<<endl;
    if(cache.size()==0){
      flag=1;
      break;
    }

      if(cache[i]==s){
      flag=0;
      pos =i;
      break;
    }
  }

  if(flag==1){
    std::cout<<"VNF not there in cache\n";
    if(cache.size()==cachesize){
      int pos1 = 0;
      minrank = rank[cache[0]];
      for(int i=1;i<cachesize;i++){
        if(rank[cache[i]]<minrank){
          minrank  = rank[cache[i]];
          pos1 = i;
        }
      }
      std::string temp = cache[pos1];
      cache[pos1] = cache[cachesize-1];
      cache[cachesize-1] = temp;

      location[cache[cache.size()-1]] = 2;
      cache.pop_back();
      std::vector<std::string>::iterator it;

        it = cache.begin();
      cache.insert(it,s);
      location[s] = 3;
      for(int i=0;i<cache.size();i++){
        rank[cache[i]] = rank[cache[i]] - minrank;
      }
      std::cout<<"cache was full least rank removed\n";

      
    }else if(cache.empty()){
      std::cout<<"cache is empty first insertion\n";
      cache.push_back(s);
      location[s] =3;
      
    }else{
      std::cout<<"cache is not full inserted at beginning\n";
        std::vector<std::string>::iterator it;
      it = cache.begin();
      cache.insert(it,s);
      location[s]=3;
      

    }
  }else{

    std::cout<<"vnf present in cache moved to front\n";
    rank[cache[pos]] = clonetime[cache[pos]]*cloneweight + priority[cache[pos]]*prweight; 
  }

}


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


  std::string s;

  
  std::map<std::string, int> clonetime;
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
  srand(time(NULL));
  cache.clear();
  for(int i=0;i<v.size();i++){
    clonetime[v[i]] = (rand()%3)*10;
    priority[v[i]] = (rand()%7)*10;
              //created a map to clone times
  }
  priority["vf1"] = 60;

  int no;
  int firewallflag=0;

  std::cout<<"Enter vnfs needed should ne less than 10\n";
  std::cin>>no;
  std::string req[no];
  double t[no];
  double simulationtime=0,maxalivetime=0;
  std::cout<<"Enter vnf name and alive times of each\n";
  for(int i=0;i<no;i++){
    std::cin>>req[i];
    std::cin>>t[i];
    rank[req[i]] = clonetime[req[i]]*cloneweight + priority[req[i]]*prweight; 
    if(rank[req[i]] < minrank){
      minrank = rank[req[i]];
    }
    if(req[i]=="vf1"){
      firewallflag=1;
    }
    if(t[i]>maxalivetime){
      maxalivetime =t[i];
    }
    simulationtime += t[i];
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

      for(int i=0;i<no;i++){
        update(req[i]);
      }
      for(int i=0;i<no;i++){
         uint16_t port =9+i;
      if(location[req[i]]==2){
        
        

        BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (addrA, port));


        source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
        ApplicationContainer sourceApps = source.Install (c.Get(2));
      sourceApps.Start (Seconds (0.0));
        sourceApps.Stop (Seconds (t[i]));

        PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
        ApplicationContainer sinkApps = sink.Install (c.Get(0));
        sinkApps.Start (Seconds (0.0));
        sinkApps.Stop (Seconds (t[i]));
      }else{

        BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (addrA, port));


        source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
        ApplicationContainer sourceApps = source.Install (c.Get(2));
      sourceApps.Start (Seconds (0.0));
        sourceApps.Stop (Seconds (clonetime[req[i]]));

        PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
        ApplicationContainer sinkApps = sink.Install (c.Get(0));
        sinkApps.Start (Seconds (0.0));
        sinkApps.Stop (Seconds (clonetime[req[i]]));

        BulkSendHelper source1 ("ns3::TcpSocketFactory",
                         InetSocketAddress (addrA, port));


        source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
        ApplicationContainer sourceApps1 = source1.Install (c.Get(3));
      sourceApps1.Start (Seconds (clonetime[req[i]]));
        sourceApps1.Stop (Seconds (t[i]));

        PacketSinkHelper sink1 ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
        ApplicationContainer sinkApps1 = sink1.Install (c.Get(0));
        sinkApps1.Start (Seconds (clonetime[req[i]]));
        sinkApps1.Stop (Seconds (t[i]));



      }

      FlowMonitorHelper flowmon;
        Ptr<FlowMonitor> monitor = flowmon.InstallAll();


        Simulator::Stop (Seconds (maxalivetime));
        Simulator::Run ();
        monitor->CheckForLostPackets ();
        monitor->SerializeToXmlFile("algo.flowmon", true, true);

      Simulator::Destroy ();
      }
  }else{
    for(int i=0;i<no;i++){
        update(req[i]);
      }
      int fl = 0;
      for(int i=0;i<cache.size();i++){
        if(cache[i]=="vf1"){
          fl=1;
          break;
        }
      }

      if(fl==1)
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
            if(req[i]!="vf1"){
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
          monitor->SerializeToXmlFile("algo.flowmon", true, true);

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
        monitor->SerializeToXmlFile("algo.flowmon", true, true);

      Simulator::Destroy ();



      }


  }



  return 0;
}