/*RAZA
 * ping.cc
 * Copyright (C) 2000 by the University of Southern California
 * $Id: ping.cc,v 1.8 2005/08/25 18:58:01 johnh Exp $
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 * The copyright of this module includes the following
 * linking-with-specific-other-licenses addition:
 *
 * In addition, as a special exception, the copyright holders of
 * this module give you permission to combine (via static or
 * dynamic linking) this module with free software programs or
 * libraries that are released under the GNU LGPL and with code
 * included in the standard release of ns-2 under the Apache 2.0
 * license or under otherwise-compatible licenses with advertising
 * requirements (or modified versions of such code, with unchanged
 * license).  You may copy and distribute such a system following the
 * terms of the GNU GPL for this module and the licenses of the
 * other code concerned, provided that you include the source code of
 * that other code when and as the GNU GPL requires distribution of
 * source code.
 *
 * Note that people who make modified versions of this module
 * are not obligated to grant this special exception for their
 * modified versions; it is their choice whether to do so.  The GNU
 * General Public License gives permission to release a modified
 * version without this exception; this exception also makes it
 * possible to release a modified version which carries forward this
 * exception.
 *
 */

// $Header: /cvsroot/nsnam/ns-2/apps/ping.cc,v 1.8 2005/08/25 18:58:01 johnh Exp $

/*
 * File: Code for a new 'Ping' Agent Class for the ns
 *       network simulator
 * Author: Marc Greis (greis@cs.uni-bonn.de), May 1998
 *
 * IMPORTANT: Incase of any changes made to this file , 
 * tutorial/examples/ping.cc file (used in Greis' tutorial) should
 * be updated as well.
 */

#include "ping.h"
#include <iostream>
 using namespace std;
//#include "/home/raza/ns-allinone-2.35/ns-2.35/queue/queue.h"
int hdr_ping::offset_;
static class PingHeaderClass : public PacketHeaderClass {
public:
	PingHeaderClass() : PacketHeaderClass("PacketHeader/Ping", 
					      sizeof(hdr_ping)) {
		bind_offset(&hdr_ping::offset_);
	}
} class_pinghdr;


static class PingClass : public TclClass {
public:
	PingClass() : TclClass("Agent/Ping") {}
	TclObject* create(int, const char*const*) {
		return (new PingAgent());
	}
} class_ping;


PingAgent::PingAgent() : Agent(PT_PING), seq(0), oneway(0)
{
	bind("packetSize_", &size_);
}

int PingAgent::command(int argc, const char*const* argv)
{
  	if(0){
      
      //nsaddr_t my_ard = addr();
      printf("PRINTING ADDRESSES");
      printf("%d\n", here_.addr_);
      printf("%d\n", dst_.addr_);
    }
    if (0) {
    //printf("RAZA");
  	Node *thisnode = Node::get_node_by_address(0);
  	//printf("%d\n",thisnode->address_);
  	linklist_head link = thisnode->linklisthead_;
    LinkHead *lhp = link.lh_first;
    if (lhp!=NULL)
      printf("THIS IS NULL");
    //Node * th1 = lhp->node();
    //printf("%d\n",th1->address_); 
    NsObject* thisObject = thisnode->intf_to_target(0);


    RoutingModule* rtm = thisnode->rtnotif_;
    Node * th = rtm->node();
    //Tcl& tcl = Tcl::instance();
    Classifier* cc  =  rtm->classifier_;
    printf("%d\n", cc->nsize_);
    NsObject * nss = cc->slot_[0];

    int ra = (((Queue*)(nss))->qlim_);
    //printf ()
    //printf("%d\n",ra);
  

     Queue* qq = globalQueue1;

    if (qq==NULL)
       printf("EMPTY");

    printf("%d\n", qq->qlim_);
    qq->printStats();
    if (global_q_head!=NULL){
      GlobalQueue* quee = global_q_head;
      int i = 0;
      while(  quee !=NULL )
      {
        i++;
        printf("%d\n", quee->que->qlim_);
        quee = quee->next;

      }
      printf("TOTAL QUEUES ARE : ");
      printf("%d\n", i);
    }

    GlobalAgent* stt = global_a_head;
    int j=0;
    while(stt!=NULL){
      j++;
      // printf("%d\n",stt->agent->here_.addr_);
      // printf("%d\n",stt->agent->here_.port_);
      // printf("%d\n",stt->agent->dst_.addr_);
      // printf("%d\n",stt->agent->dst_.port_);
      // printf("-------------------------");      
      stt=stt->next;
    }
    printf("TOTAL AGENTS ARE");
    printf("%d\n", j);

  }
	if (argc == 2) {
    if (strcmp(argv[1], "send") == 0) {
      // Create a new packet
      Packet* pkt = allocpkt();
      // Access the Ping header for the new packet:
      hdr_ping* hdr = hdr_ping::access(pkt);
      // Set the 'ret' field to 0, so the receiving node
      // knows that it has to generate an echo packet
      hdr->ret = 0;
      hdr->seq = seq++;
      hdr->link_num = 0;
      hdr->utilization = -1;
      hdr->q_lim = -1;
      hdr->no_flows = -1;
      hdr->q_len = -1;
      // Store the current time in the 'send_time' field
      hdr->send_time = Scheduler::instance().clock();
      // Send the packet
      send(pkt, 0);
      // return TCL_OK, so the calling function knows that
      // the command has been processed
      return (TCL_OK);
    
    }
    else if (strcmp(argv[1], "qlen") == 0) {
      // HP
      Packet* pkt = allocpkt();
      hdr_ping* hdr = hdr_ping::access(pkt);
      hdr->ret = 0;
      hdr->seq = seq++;
      hdr->link_num = 0;
      hdr->utilization = -1;
      hdr->q_lim = -1;
      hdr->no_flows = -1;
      hdr->q_len = -1;
      hdr->statDemanded = QLEN;
      // Store the current time in the 'send_time' field
      hdr->send_time = Scheduler::instance().clock();
      // Send the packet
      send(pkt, 0);
      // return TCL_OK, so the calling function knows that
      // the command has been processed
      return (TCL_OK);


    }
    else if (strcmp(argv[1], "utilization") == 0) {
      // HP
      Packet* pkt = allocpkt();
      hdr_ping* hdr = hdr_ping::access(pkt);
      hdr->ret = 0;
      hdr->seq = seq++;
      hdr->link_num = 0;
      hdr->utilization = -1;
      hdr->q_lim = -1;
      hdr->no_flows = -1;
      hdr->q_len = -1;
      hdr->statDemanded = UTILIZATION;
      // Store the current time in the 'send_time' field
      hdr->send_time = Scheduler::instance().clock();
      // Send the packet
      send(pkt, 0);
      // return TCL_OK, so the calling function knows that
      // the command has been processed
      return (TCL_OK);
    }
    else if (strcmp(argv[1], "numFlows") == 0) {
      //cout << "NUMFLOWS demanded " << endl;
      // HP
      Packet* pkt = allocpkt();
      hdr_ping* hdr = hdr_ping::access(pkt);
      hdr->ret = 0;
      hdr->seq = seq++;
     // hdr->link_num = 0;
      hdr->utilization = -1;
      hdr->q_lim = -1;
      hdr->no_flows = -1;
      hdr->q_len = -1;
      hdr->statDemanded = NUMFLOWS;
      // Store the current time in the 'send_time' field
      hdr->send_time = Scheduler::instance().clock();
      //cout << "statDemanded = " << hdr->statDemanded << endl;
      // Send the packet
      send(pkt, 0);
      // return TCL_OK, so the calling function knows that
      // the command has been processed
      return (TCL_OK);
    }
    
    else if (strcmp(argv[1], "start-WL-brdcast") == 0) {
      Packet* pkt = allocpkt();
      
      hdr_ip* iph = HDR_IP(pkt);
      hdr_ping* ph = hdr_ping::access(pkt);
      
      iph->daddr() = IP_BROADCAST;
      iph->dport() = iph->sport();
      ph->ret = 0;
      send(pkt, (Handler*) 0);
      return (TCL_OK);
    }

    else if (strcmp(argv[1], "oneway") == 0) {
      oneway=1;
      return (TCL_OK);
    }
  }
  
  // If the command hasn't been processed by PingAgent()::command,
  // call the command() function for the base class
  return (Agent::command(argc, argv));
}


  /*
  eSDN. invoked in Raza.cc and tcp.cc
  HP
  */
  void PingAgent::send_c(int sd,int lN){

      Packet* pkt = allocpkt();
      hdr_ping* hdr = hdr_ping::access(pkt);
      hdr->ret = 3; // set intentionally so that receiver knows i have to respond. receiver sets it 4 so i know this is my response. 
      hdr->seq = seq++;
      hdr->link_num = lN;
      hdr->utilization = -1;
      hdr->q_lim = -1;
      hdr->no_flows = -1;
      hdr->q_len = -1;
      hdr->statDemanded = sd;
      hdr->send_time = Scheduler::instance().clock();
      send(pkt, 0);
  }

/*
eSDN. called by receiver. 
*/

void PingAgent::updateStats(hdr_ping* hdr){
 // cout << "MY NUMBER IS " << addr() <<endl;
  int link_num = hdr->link_num;
  // do the mapping of the link with the auxilary data structure
  // to get the queue array number 
  //cout << "Raza wants big L = " << link_num << endl;
  GlobalQueue* gQ = global_q_head;
  //find the relevant queue here
  for (int i=0;i<link_num;i++) {
  	gQ = gQ->next;
  }
//  printf("THE Queue size is %d\n",gQ->que->totalBytes());

  hdr->utilization = gQ->que->cur_util_;
  hdr->q_lim = gQ->que->qlim_;
  //hdr->q_len = gQ->que->byteLength();
  if (hdr->statDemanded==NUMFLOWS){
    hdr->no_flows = gQ->que->num_flows();
    //printf("IN NUM FLOWS\n");
    return;
  }
    
   if (hdr->statDemanded==UTILIZATION) {
    hdr->utilization = gQ->que->cur_util_;
    return;
  }
  else if (hdr->statDemanded==QLEN) {
    hdr->q_len = gQ->que->byteLength();
    //printf("IN QUEUE LENGTH\n");
    return;
  }

}



void PingAgent::recv(Packet* pkt, Handler*)
{
  // Access the IP header for the received packet:
  hdr_ip* hdrip = hdr_ip::access(pkt);
  
  // Access the Ping header for the received packet:
  hdr_ping* hdr = hdr_ping::access(pkt);
  

  // check if in brdcast mode
  if ((u_int32_t)hdrip->daddr() == IP_BROADCAST) {
    if (hdr->ret == 0) {
      
      printf("Recv BRDCAST Ping REQ : at %d.%d from %d.%d\n", here_.addr_, here_.port_, hdrip->saddr(), hdrip->sport());
      Packet::free(pkt);
      
      // create reply
      Packet* pktret = allocpkt();

      hdr_ping* hdrret = hdr_ping::access(pktret);
      hdr_ip* ipret = hdr_ip::access(pktret);
      
      hdrret->ret = 1;
      
      // add brdcast address
      ipret->daddr() = IP_BROADCAST;
      ipret->dport() = ipret->sport();

      send(pktret, 0);
    
    } else {
      printf("Recv BRDCAST Ping REPLY : at %d.%d from %d.%d\n", here_.addr_, here_.port_, hdrip->saddr(), hdrip->sport());
      Packet::free(pkt);
    }
    return;
  }
  // Is the 'ret' field = 0 (i.e. the receiving node is being pinged)?

  //eSDN
  if (hdr->ret == 3) { 
    // Send an 'echo'. First save the old packet's send_time

//    cout<<"Agent PING is called from "<< dst_.addr_ << " to " <<  here_.addr_<< " for link " << hdr->link_num << " with status " <<endl;



    double stime = hdr->send_time;
    int rcv_seq = hdr->seq;
    // Discard the packet
    int n = hdr->link_num;
    //cout << "n = " << n << endl;
    int stat_dem = hdr->statDemanded;
    Packet::free(pkt);
    
    // Create a new packet
    Packet* pktret = allocpkt();
    // Access the Ping header for the new packet:
    hdr_ping* hdrret = hdr_ping::access(pktret);
    // Set the 'ret' field to 1, so the receiver won't send
    // another echo
    hdrret->ret = 4;
    hdrret->link_num = n;
    hdrret->statDemanded = stat_dem;
    updateStats(hdrret);
    // Set the send_time field to the correct value
    hdrret->send_time = stime;
    // Added by Andrei Gurtov for one-way delay measurement.
    hdrret->rcv_time = Scheduler::instance().clock();
    hdrret->seq = rcv_seq; // 
    // Send the packet
    send(pktret, 0); // sent back. 
  } else if(hdr->ret == 4) { //response of the poll query. 

  	Node* me = Node::get_node_by_address(addr());
    if (hdr->statDemanded == NUMFLOWS)
  	 me->topo_links[hdr->link_num].stats.setStats_Num(hdr->utilization,hdr->q_lim,hdr->no_flows); // topolinks has link objects. 
  	else if (hdr->statDemanded == QLEN) // not correct atm. 
     me->topo_links[hdr->link_num].stats.setStats_Queue(hdr->utilization,hdr->q_lim,hdr->q_len);


    Packet::free(pkt);
    return;
  }

  if (hdr->ret == 0) {
    // Send an 'echo'. First save the old packet's send_time
    double stime = hdr->send_time;
    int rcv_seq = hdr->seq;
    // Discard the packet
        Packet::free(pkt);
    // Create a new packet
    Packet* pktret = allocpkt();
    // Access the Ping header for the new packet:
    hdr_ping* hdrret = hdr_ping::access(pktret);
    // Set the 'ret' field to 1, so the receiver won't send
    // another echo
    hdrret->ret = 1;
    hdrret->statDemanded = hdr->statDemanded;
    Packet::free(pkt);
    updateStats(hdrret);
    // Set the send_time field to the correct value
    hdrret->send_time = stime;
    // Added by Andrei Gurtov for one-way delay measurement.
    hdrret->rcv_time = Scheduler::instance().clock();
    hdrret->seq = rcv_seq;
    // Send the packet
    send(pktret, 0);
  } else if (hdr->ret == 1) {

    char out[100];

    if (oneway) //AG
      	sprintf(out, "%s recv %d %d %3.1f %3.1f", name(), 
	    hdrip->src_.addr_ >> Address::instance().NodeShift_[1],
	    hdr->seq, (hdr->rcv_time - hdr->send_time) * 1000,
	    (Scheduler::instance().clock()-hdr->rcv_time) * 1000);
    else sprintf(out, "%s recv %d %3.1f", name(), 
	    hdrip->src_.addr_ >> Address::instance().NodeShift_[1],
	    (Scheduler::instance().clock()-hdr->send_time) * 1000);
    Tcl& tcl = Tcl::instance();
    tcl.eval(out);
    // Discard the packet
    Packet::free(pkt);
  }
}


