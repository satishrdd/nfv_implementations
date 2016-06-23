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
#define cachesize 50

#define prweight 7
#define cloneweight 3


int hitrate =0;


double minrank=0; 
  using namespace ns3;


std::vector<std::string> cache1;
std::vector<std::string> cache2;
std::vector<std::string> v;
std::map<std::string, int> clonetime;
std::map<std::string, int> priority;
std::map<std::string, int> location1;
std::map<std::string, int> location2;
std::map<std::string, double> rank;
std::map<std::string,int> traffic;//0 for tcp 1 for udp
std::map<std::string,int> heaviness;//0 for heavy,1 for medium ,2 for light

template <typename T>
  std::string NumberToString ( T Number )
  {
     std::ostringstream ss;
     ss << Number;
     return ss.str();
  }

void update(std::string s,int userflag){
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

	if(userflag==1){
	for(int i=0;i<cache1.size();i++){
		//cout<<"re"<<endl;
		if(cache1.size()==0){
			flag=1;
			break;
		}

			if(cache1[i]==s){
			flag=0;
			pos =i;
			break;
		}
	}}else{
	for(int i=0;i<cache2.size();i++){
		//cout<<"re"<<endl;
		if(cache2.size()==0){
			flag=1;
			break;
		}

			if(cache2[i]==s){
			flag=0;
			pos =i;
			break;
		}
	}	
	}
	if(userflag==1){
	if(flag==1){
		std::cout<<"VNF not there in cache\n";
		
		if(cache1.size()==cachesize){
			location1[cache1[cache1.size()-1]]=3;
			location1[s] =2;
			cache1.pop_back();
			std::vector<std::string>::iterator it;

  it = cache1.begin();
			cache1.insert(it,s);
			
			std::cout<<"cache was full least recent removed\n";
			
		}else if(cache1.empty()){
			std::cout<<"cache is empty first insertion\n";
			cache1.push_back(s);
			location1[s] =2;
		}else{
			std::cout<<"cache is not full inserted at beginning\n";
			  std::vector<std::string>::iterator it;
			it = cache1.begin();
			cache1.insert(it,s);
			location1[s]=2;			

		}
	}else{

		std::cout<<"vnf present in cache moved to front\n";
		for(int i=pos;i>0;i--){
			std::string temp = cache1[i];
			cache1[i] = cache1[i-1];
			cache1[i-1] = temp;
		}
		hitrate++;
	}
}else{
	if(flag==1){
		std::cout<<"VNF not there in cache\n";
		
		if(cache2.size()==cachesize){
			location2[cache2[cache2.size()-1]]=3;
			location2[s] =2;
			cache2.pop_back();
			std::vector<std::string>::iterator it;

  it = cache2.begin();
			cache2.insert(it,s);
			
			std::cout<<"cache was full least recent removed\n";
			
		}else if(cache2.empty()){
			std::cout<<"cache is empty first insertion\n";
			location2[s] =2;
			cache2.push_back(s);
			
		}else{
			std::cout<<"cache is not full inserted at beginning\n";
			  std::vector<std::string>::iterator it;
			it = cache2.begin();
			cache2.insert(it,s);
			location2[s] =2;

		}
	}else{

		std::cout<<"vnf present in cache moved to front\n";
		for(int i=pos;i>0;i--){
			std::string temp = cache2[i];
			cache2[i] = cache2[i-1];
			cache2[i-1] = temp;
		}
		hitrate++;
	}
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
  for(int i=1;i<=40;i++){
    v.push_back("v"+NumberToString(i));
    heaviness["v"+NumberToString(i)] = 0;
    if(i<=20){
    traffic["v"+NumberToString(i)] = 1;
    }else{
      traffic["v"+NumberToString(i)] = 2;
    }
  }

  v.push_back("vf1");
  for(int i=41;i<=80;i++){
    v.push_back("v"+NumberToString(i));
    heaviness["v"+NumberToString(i)] = 99;
      if(i<=60){
    traffic["v"+NumberToString(i)] = 1;
    }else{
      traffic["v"+NumberToString(i)] = 2;
    }
  }
  v.push_back("vf2");
  for(int i=81;i<=100;i++){
    v.push_back("v"+NumberToString(i));
    heaviness["v"+NumberToString(i)] = 9999;
      if(i<=90){
    traffic["v"+NumberToString(i)] = 1;
    }else{
      traffic["v"+NumberToString(i)] = 2;
    }
  }

	srand(time(NULL));
	cache1.clear();
  cache2.clear();
	for(int i=0;i<v.size();i++){
		clonetime[v[i]] = (rand()%3)*10;
		
							//created a map to clone times
	}
	priority["vf1"] = 60;

	int no;
	int firewallflag=0;

	std::cout<<"Enter vnfs needed should ne less than 10\n";
	std::cin>>no;
	std::string req[no];
	double t[no];
	int user[no];
	double simulationtime=0,maxalivetime=0;
	std::cout<<"Enter vnf name and alive times of each and user no requesting\n";
	for(int i=0;i<no;i++){
		std::cin>>req[i];
		std::cin>>t[i];
		std::cin>>user[i];
		
		if(req[i]=="vf1"){
			firewallflag=1;
		}
		if(t[i]>maxalivetime){
			maxalivetime =t[i];
		}
		simulationtime += t[i];
	}




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


     NodeContainer c1 = NodeContainer(nu1,r1,nC,ncache1);
    NodeContainer c2 =NodeContainer(nu2,r2,nC,ncache2);

    InternetStackHelper internet ;
   

    NodeContainer rnC = NodeContainer(r,nC);
    NodeContainer rr1 = NodeContainer(r,r1);
    NodeContainer rr2 = NodeContainer(r,r2);
    NodeContainer r1ncache1 = NodeContainer(r1,ncache1);
    NodeContainer r2ncache2 = NodeContainer(r2,ncache2);
    NodeContainer r1nu1 = NodeContainer(r1,nu1);
    NodeContainer r2nu2  = NodeContainer(r2,nu2);
    NodeContainer u1u2 = NodeContainer(nu1,nu2);

   internet.Install(rnC);
  
   internet.Install(r1ncache1);
   internet.Install(r2ncache2);
   internet.Install(u1u2);


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



  if(firewallflag==0){

      for(int i=0;i<no;i++){
        update(req[i],user[i]);
      }
      for(int i=0;i<no;i++){
         uint16_t port =9+i;
         uint16_t port1 =9+i;
      if(((user[i]==1)&&(location1[req[i]]==2))||((user[i]==2)&&(location2[req[i]]==2))){
        
        
      if(user[i]==1){
        

        if(traffic[req[i]]==1){
        BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (addru1, port));


        source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
         source.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
        ApplicationContainer sourceApps = source.Install (c1.Get(2));
      sourceApps.Start (Seconds (0.0));
        sourceApps.Stop (Seconds (t[i]));

        PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
        ApplicationContainer sinkApps = sink.Install (c1.Get(0));
        sinkApps.Start (Seconds (0.0));
        sinkApps.Stop (Seconds (t[i]));
        
      }else{
        UdpServerHelper server (port);
        ApplicationContainer apps = server.Install (c1.Get (2));
        apps.Start (Seconds (0.0));
       apps.Stop (Seconds (t[i]));
        uint32_t MaxPacketSize = 1024;
        double inter;
        if(heaviness[req[i]]==0){
          inter = 0.0001;
        }else if(heaviness[req[i]]==1){
          inter = 0.01;
        }else{
          inter = 0.1;
        }
       Time interPacketInterval = Seconds (inter);
       uint32_t maxPacketCount = t[i]/inter;
       UdpClientHelper client (addrc, port);
        client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps = client.Install (c1.Get (0));
        apps.Start (Seconds (0.0));
        apps.Stop (Seconds (t[i]));
        }



        }else{
          if(traffic[req[i]]==1){
          BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (addru2, port));


        source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
        source.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
        ApplicationContainer sourceApps = source.Install (c2.Get(2));
      sourceApps.Start (Seconds (0.0));
        sourceApps.Stop (Seconds (t[i]));

        PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
        ApplicationContainer sinkApps = sink.Install (c2.Get(0));
        sinkApps.Start (Seconds (0.0));
        sinkApps.Stop (Seconds (t[i]));
        }else{
        UdpServerHelper server (port);
        ApplicationContainer apps = server.Install (c2.Get (2));
        apps.Start (Seconds (0.0));
       apps.Stop (Seconds (t[i]));
        uint32_t MaxPacketSize = 1024;
        double inter;
        if(heaviness[req[i]]==0){
          inter = 0.0001;
        }else if(heaviness[req[i]]==1){
          inter = 0.01;
        }else{
          inter = 0.1;
        }
       Time interPacketInterval = Seconds (inter);
       uint32_t maxPacketCount = t[i]/inter;
       UdpClientHelper client (addrc, port);
        client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps = client.Install (c2.Get (0));
        apps.Start (Seconds (0.0));
        apps.Stop (Seconds (t[i]));
        }
    }
      }else{
        if(user[i]==1){
          if(traffic[req[i]]==0){
        BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (addru1, port));


        source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
        source.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
        ApplicationContainer sourceApps = source.Install (c1.Get(2));

      sourceApps.Start (Seconds (0.0));
        sourceApps.Stop (Seconds (clonetime[req[i]]));

        PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
        ApplicationContainer sinkApps = sink.Install (c1.Get(0));
        sinkApps.Start (Seconds (0.0));
        sinkApps.Stop (Seconds (clonetime[req[i]]));

        BulkSendHelper source1 ("ns3::TcpSocketFactory",
                         InetSocketAddress (addru1, port));


        source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
        source1.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
        ApplicationContainer sourceApps1 = source1.Install (c1.Get(3));
      sourceApps1.Start (Seconds (clonetime[req[i]]));
        sourceApps1.Stop (Seconds (t[i]));

        PacketSinkHelper sink1 ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
        ApplicationContainer sinkApps1 = sink1.Install (c1.Get(0));
        sinkApps1.Start (Seconds (clonetime[req[i]]));
        sinkApps1.Stop (Seconds (t[i]));}
        else{
        UdpServerHelper server (port);
        ApplicationContainer apps = server.Install (c1.Get (2));
        apps.Start (Seconds (0.0));
       apps.Stop (Seconds (clonetime[req[i]]));
        uint32_t MaxPacketSize = 1024;
        double inter;
        if(heaviness[req[i]]==0){
          inter = 0.0001;
        }else if(heaviness[req[i]]==1){
          inter = 0.01;
        }else{
          inter = 0.1;
        }
       Time interPacketInterval = Seconds (inter);
       uint32_t maxPacketCount = clonetime[req[i]]/inter;
       UdpClientHelper client (addrc, port);
        client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps = client.Install (c1.Get (0));
        apps.Start (Seconds (0.0));
        apps.Stop (Seconds (clonetime[req[i]]));

        UdpServerHelper server1 (port1);
        ApplicationContainer apps1 = server1.Install (c1.Get (3));
        apps1.Start (Seconds (clonetime[req[i]]));
       apps1.Stop (Seconds (t[i]));
        maxPacketCount = t[i]/inter;
       UdpClientHelper client1 (addrcache1, port);
        client1.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client1.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client1.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps1 = client1.Install (c1.Get (0));
        apps1.Start (Seconds (0.0));
        apps1.Stop (Seconds (t[i]));
        }

      }else{
        if(traffic[req[i]]==1){
        BulkSendHelper source ("ns3::TcpSocketFactory",
                         InetSocketAddress (addru2, port));


        source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
        source.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
        ApplicationContainer sourceApps = source.Install (c2.Get(2));
      sourceApps.Start (Seconds (0.0));
        sourceApps.Stop (Seconds (clonetime[req[i]]));

        PacketSinkHelper sink ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
        ApplicationContainer sinkApps = sink.Install (c2.Get(0));
        sinkApps.Start (Seconds (0.0));
        sinkApps.Stop (Seconds (clonetime[req[i]]));

        BulkSendHelper source1 ("ns3::TcpSocketFactory",
                         InetSocketAddress (addru2, port));


        source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
        source1.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
        ApplicationContainer sourceApps1 = source1.Install (c2.Get(3));
      sourceApps1.Start (Seconds (clonetime[req[i]]));
        sourceApps1.Stop (Seconds (t[i]));

        PacketSinkHelper sink1 ("ns3::TcpSocketFactory",
                         InetSocketAddress (Ipv4Address::GetAny (), port));
        ApplicationContainer sinkApps1 = sink1.Install (c2.Get(0));
        sinkApps1.Start (Seconds (clonetime[req[i]]));
        sinkApps1.Stop (Seconds (t[i]));
      }else{
        UdpServerHelper server (port);
        ApplicationContainer apps = server.Install (c2.Get (2));
        apps.Start (Seconds (0.0));
       apps.Stop (Seconds (clonetime[req[i]]));
        uint32_t MaxPacketSize = 1024;
        double inter;
        if(heaviness[req[i]]==0){
          inter = 0.0001;
        }else if(heaviness[req[i]]==1){
          inter = 0.01;
        }else{
          inter = 0.1;
        }
       Time interPacketInterval = Seconds (inter);
       uint32_t maxPacketCount = clonetime[req[i]]/inter;
       UdpClientHelper client (addrc, port);
        client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps = client.Install (c2.Get (0));
        apps.Start (Seconds (0.0));
        apps.Stop (Seconds (clonetime[req[i]]));

        UdpServerHelper server1 (port1);
        ApplicationContainer apps1 = server1.Install (c2.Get (3));
        apps1.Start (Seconds (clonetime[req[i]]));
       apps1.Stop (Seconds (t[i]));
        maxPacketCount = t[i]/inter;
       UdpClientHelper client1 (addrcache2, port);
        client1.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client1.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client1.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps1 = client1.Install (c2.Get (0));
        apps1.Start (Seconds (0.0));
        apps1.Stop (Seconds (t[i]));
        }
      }

      }


      
      }
      FlowMonitorHelper flowmon;
        Ptr<FlowMonitor> monitor = flowmon.InstallAll();


        Simulator::Stop (Seconds (2*maxalivetime));
        Simulator::Run ();
        monitor->CheckForLostPackets ();
        monitor->SerializeToXmlFile("online.flowmon", true, true);

      Simulator::Destroy ();
  }else{
    for(int i=0;i<no;i++){
        update(req[i],user[i]);
      }
      int fl = 0;
      for(int i=0;i<cache1.size();i++){
        if(cache1[i]=="vf1"||cache1[i]=="vf2"){
          fl=1;
          break;
        }
      }
      for(int i=0;i<cache2.size();i++){
        if(cache2[i]=="vf1"||cache2[i]=="vf2"){
          fl=2;
          break;
        }
      }
     
  
          for(int i=0;i<no;i++){
            if(((fl==1)&&(user[i]==1))||((fl==2)&&(user[i]==2))) {
            if(req[i]!="vf1"&&req[i]!="vf2"){
              if(user[i]==1){
            uint16_t port =9+i,port1=45+i;
            if(traffic[req[i]]==1){
                BulkSendHelper source ("ns3::TcpSocketFactory",InetSocketAddress (addrcache1, port));
                        source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
                        source.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
              ApplicationContainer sourceApps = source.Install (c1.Get(location1[req[i]]));
              sourceApps.Start (Seconds (0.0));
              sourceApps.Stop (Seconds (t[i]));

              PacketSinkHelper sink ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port));
              ApplicationContainer sinkApps = sink.Install (ncache1);
              sinkApps.Start (Seconds (0.0));
              sinkApps.Stop (Seconds (t[i]));

            BulkSendHelper source1 ("ns3::TcpSocketFactory", InetSocketAddress (addru1, port1));
            source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
            source1.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
              ApplicationContainer sourceApps1 = source1.Install (ncache1);
              sourceApps1.Start (Seconds (t[i]+1));
              sourceApps1.Stop (Seconds (2*t[i]));

              PacketSinkHelper sink1 ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port1));
              ApplicationContainer sinkApps1 = sink1.Install (nu1); 
              sinkApps1.Start (Seconds (t[i]+1));
              sinkApps1.Stop (Seconds (2*t[i]));
            }else{
              Address serveraddr;
        UdpServerHelper server (port);
        ApplicationContainer apps = server.Install (c1.Get (location1[req[i]]));
        apps.Start (Seconds (0.0));
       apps.Stop (Seconds (t[i]));
        uint32_t MaxPacketSize = 1024;
        double inter;
        if(heaviness[req[i]]==0){
          inter = 0.0001;
        }else if(heaviness[req[i]]==1){
          inter = 0.01;
        }else{
          inter = 0.1;
        }
        if(location1[req[i]]==2){
          serveraddr = addrc;
        }else{
          serveraddr = addrcache1;
        }
       Time interPacketInterval = Seconds (inter);
       uint32_t maxPacketCount = t[i]/(inter);
       UdpClientHelper client (serveraddr, port);
        client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps = client.Install (ncache1);
        apps.Start (Seconds (0.0));
        apps.Stop (Seconds (t[i]));

        UdpServerHelper server1 (port1);
        ApplicationContainer apps1 = server1.Install (c1.Get (3));
        apps1.Start (Seconds (t[i]+1));
       apps1.Stop (Seconds (2*t[i]));
        maxPacketCount = t[i]/(inter);
       UdpClientHelper client1 (addrcache1, port);
        client1.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client1.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client1.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps1 = client1.Install (c1.Get (0));
        apps1.Start (Seconds (t[i]+1));
        apps1.Stop (Seconds (2*t[i]));
        }
         }else{
            if(traffic[req[i]]==1){
              uint16_t port =9+i,port1=45+i;

                BulkSendHelper source ("ns3::TcpSocketFactory",InetSocketAddress (addrcache2, port));
                        source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
                        source.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
              ApplicationContainer sourceApps = source.Install (c2.Get(location2[req[i]]));
              sourceApps.Start (Seconds (0.0));
              sourceApps.Stop (Seconds (t[i]));

              PacketSinkHelper sink ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port));
              ApplicationContainer sinkApps = sink.Install (ncache2);
              sinkApps.Start (Seconds (0.0));
              sinkApps.Stop (Seconds (t[i]));

            BulkSendHelper source1 ("ns3::TcpSocketFactory", InetSocketAddress (addru2, port1));
            source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
            source1.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
              ApplicationContainer sourceApps1 = source1.Install (ncache2);
              sourceApps1.Start (Seconds (t[i]+1));
              sourceApps1.Stop (Seconds (2*t[i]));

              PacketSinkHelper sink1 ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port1));
              ApplicationContainer sinkApps1 = sink1.Install (nu2); 
              sinkApps1.Start (Seconds (t[i]+1));
              sinkApps1.Stop (Seconds (2*t[i]));
            }else{
                uint16_t port =9+i,port1=45+i;
              Address serveraddr;
        UdpServerHelper server (port);
        ApplicationContainer apps = server.Install (c2.Get (location2[req[i]]));
        apps.Start (Seconds (0.0));
       apps.Stop (Seconds (t[i]));
        uint32_t MaxPacketSize = 1024;
        double inter;
        if(heaviness[req[i]]==0){
          inter = 0.0001;
        }else if(heaviness[req[i]]==1){
          inter = 0.01;
        }else{
          inter = 0.1;
        }
        if(location2[req[i]]==2){
          serveraddr = addrc;
        }else{
          serveraddr = addrcache2;
        }
       Time interPacketInterval = Seconds (inter);
       uint32_t maxPacketCount = t[i]/(inter);
       UdpClientHelper client (serveraddr, port);
        client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps = client.Install (ncache2);
        apps.Start (Seconds (0.0));
        apps.Stop (Seconds (t[i]));

        UdpServerHelper server1 (port1);
        ApplicationContainer apps1 = server1.Install (c2.Get (3));
        apps1.Start (Seconds (t[i]+1));
       apps1.Stop (Seconds (2*t[i]));
        maxPacketCount = t[i]/(inter);
       UdpClientHelper client1 (addrcache2, port);
        client1.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client1.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client1.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps1 = client1.Install (c2.Get (0));
        apps1.Start (Seconds (t[i]+1));
        apps1.Stop (Seconds (2*t[i]));
        }



            }
          }
        }else{
            if(user[i]==1){
            if(req[i]!="vf1"&&req[i]!="vf2"){
            uint16_t port =9+i,port1=45+i;
            if(traffic[req[i]]==1){
                BulkSendHelper source ("ns3::TcpSocketFactory", InetSocketAddress (addrc, port));
            source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
            source.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
              ApplicationContainer sourceApps = source.Install (c1.Get(location1[req[i]]));
              sourceApps.Start (Seconds (0.0));
              sourceApps.Stop (Seconds (t[i]));

              PacketSinkHelper sink ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port));
              ApplicationContainer sinkApps = sink.Install (nC);
              sinkApps.Start (Seconds (0.0));
              sinkApps.Stop (Seconds (t[i]));

            BulkSendHelper source1 ("ns3::TcpSocketFactory",InetSocketAddress (addru1, port1));
            source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
            source1.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
              ApplicationContainer sourceApps1 = source1.Install (nC);
              sourceApps1.Start (Seconds (t[i]+1));
              sourceApps1.Stop (Seconds (2*t[i]));

              PacketSinkHelper sink1 ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port1));
              ApplicationContainer sinkApps1 = sink1.Install (nu1);
              sinkApps1.Start (Seconds (t[i]+1));
              sinkApps1.Stop (Seconds (2*t[i]));

            } else{
              Address serveraddr;
        UdpServerHelper server (port);
        ApplicationContainer apps = server.Install (c1.Get (location1[req[i]]));
        apps.Start (Seconds (0.0));
       apps.Stop (Seconds (t[i]));
        uint32_t MaxPacketSize = 1024;
        double inter;
        if(heaviness[req[i]]==0){
          inter = 0.0001;
        }else if(heaviness[req[i]]==1){
          inter = 0.01;
        }else{
          inter = 0.1;
        }
        if(location1[req[i]]==2){
          serveraddr = addrc;
        }else{
          serveraddr = addrcache1;
        }
       Time interPacketInterval = Seconds (inter);
       uint32_t maxPacketCount = t[i]/(inter);
       UdpClientHelper client (serveraddr, port);
        client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps = client.Install (nC);
        apps.Start (Seconds (0.0));
        apps.Stop (Seconds (t[i]));

        UdpServerHelper server1 (port1);
        ApplicationContainer apps1 = server1.Install (c1.Get (2));
        apps1.Start (Seconds (t[i]+1));
       apps1.Stop (Seconds (2*t[i]));
        maxPacketCount = t[i]/(inter);
       UdpClientHelper client1 (addrc, port);
        client1.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client1.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client1.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps1 = client1.Install (c1.Get (0));
        apps1.Start (Seconds (t[i]+1));
        apps1.Stop (Seconds (2*t[i]));
        }




          }
        }else{
          if(req[i]!="vf1"&&req[i]!="vf2"){
            uint16_t port =9+i,port1=45+i;
            if(traffic[req[i]]==1){
                BulkSendHelper source ("ns3::TcpSocketFactory", InetSocketAddress (addrc, port));
            source.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
            source.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
              ApplicationContainer sourceApps = source.Install (c2.Get(location2[req[i]]));
              sourceApps.Start (Seconds (0.0));
              sourceApps.Stop (Seconds (t[i]));

              PacketSinkHelper sink ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port));
              ApplicationContainer sinkApps = sink.Install (nC);
              sinkApps.Start (Seconds (0.0));
              sinkApps.Stop (Seconds (t[i]));

            BulkSendHelper source1 ("ns3::TcpSocketFactory",InetSocketAddress (addru2, port1));
            source1.SetAttribute ("MaxBytes", UintegerValue (maxBytes));
            source1.SetAttribute ("SendSize", UintegerValue ((heaviness[req[i]]+1)*t[i]*10000));
              ApplicationContainer sourceApps1 = source1.Install (nC);
              sourceApps1.Start (Seconds (t[i]+1));
              sourceApps1.Stop (Seconds (2*t[i]));

              PacketSinkHelper sink1 ("ns3::TcpSocketFactory",InetSocketAddress (Ipv4Address::GetAny (), port1));
              ApplicationContainer sinkApps1 = sink1.Install (nu2);
              sinkApps1.Start (Seconds (t[i]+1));
              sinkApps1.Stop (Seconds (2*t[i]));
              }else{
              Address serveraddr;
        UdpServerHelper server (port);
        ApplicationContainer apps = server.Install (c2.Get (location2[req[i]]));
        apps.Start (Seconds (0.0));
       apps.Stop (Seconds (t[i]));
        uint32_t MaxPacketSize = 1024;
        double inter;
        if(heaviness[req[i]]==0){
          inter = 0.0001;
        }else if(heaviness[req[i]]==1){
          inter = 0.01;
        }else{
          inter = 0.1;
        }
        if(location2[req[i]]==2){
          serveraddr = addrc;
        }else{
          serveraddr = addrcache2;
        }
       Time interPacketInterval = Seconds (inter);
       uint32_t maxPacketCount = t[i]/(inter);
       UdpClientHelper client (serveraddr, port);
        client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps = client.Install (nC);
        apps.Start (Seconds (0.0));
        apps.Stop (Seconds (t[i]));

        UdpServerHelper server1 (port1);
        ApplicationContainer apps1 = server1.Install (c2.Get (2));
        apps1.Start (Seconds (t[i]+1));
       apps1.Stop (Seconds (2*t[i]));
        maxPacketCount = t[i]/(inter);
       UdpClientHelper client1 (addrc, port);
        client1.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
        client1.SetAttribute ("Interval", TimeValue (interPacketInterval));
        client1.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  
        apps1 = client1.Install (c2.Get (0));
        apps1.Start (Seconds (t[i]+1));
        apps1.Stop (Seconds (2*t[i]));
        }

          }
        }
        }

          FlowMonitorHelper flowmon;
          Ptr<FlowMonitor> monitor = flowmon.InstallAll();

          Simulator::Stop (Seconds (2*maxalivetime));
          Simulator::Run ();
          monitor->CheckForLostPackets ();
          monitor->SerializeToXmlFile("online.flowmon", true, true);

      Simulator::Destroy ();

      }
      

}
	

	std::cout<<hitrate<<std::endl;

	return 0;
}