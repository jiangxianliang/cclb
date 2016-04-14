
/*
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

#include "esdndht.h"
#include <iostream>

using namespace std;
int hdr_esdndht::offset_;
static class esdndhtHeaderClass : public PacketHeaderClass {
public:
	esdndhtHeaderClass() : PacketHeaderClass("PacketHeader/esdndht",
								sizeof(hdr_esdndht)) {
		bind_offset(&hdr_esdndht::offset_);
	}
} class_esdndhthdr;


static class esdndhtClass : public TclClass {
public:
	esdndhtClass() : TclClass("Agent/esdndht") {}
	TclObject* create(int, const char*const*) {
		return (new esdndhtAgent());
	}
} class_esdndht;


esdndhtAgent::esdndhtAgent() : Agent(PT_esdndht), seq(0), oneway(0)
{
	bind("packetSize_", &size_);
}

/*
eSDN
*/
void esdndhtAgent::send_c(int sd,int lN,int status){
  Node *me = Node::get_node_by_address(addr());


	Packet* pkt = allocpkt();
	hdr_esdndht* hdr = hdr_esdndht::access(pkt);
	hdr->start = status;
	hdr->ret = status;
	hdr->seq = seq++;
	hdr->link_num = lN;

  if(status == 2){
		hdr->utilization = me->topo_links[hdr->link_num].stats.utilization;
		hdr->q_lim = me->topo_links[hdr->link_num].stats.q_lim;
		hdr->no_flows = me->topo_links[hdr->link_num].stats.no_flows;
		hdr->q_len = me->topo_links[hdr->link_num].stats.q_size;
	}
	else{
    hdr->utilization = -1;
    hdr->q_lim = -1;
    hdr->no_flows = -1;
    hdr->q_len = -1;
	}
	hdr->statDemanded = sd;
	hdr->send_time = Scheduler::instance().clock();
	send(pkt, 0);
}




int esdndhtAgent::command(int argc, const char*const* argv)
{
	if (argc == 2) {
		if (strcmp(argv[1], "send") == 0) {
			// Create a new packet
			Packet* pkt = allocpkt();
			// Access the Ping header for the new packet:
			hdr_esdndht* hdr = hdr_esdndht::access(pkt);
			// Set the 'ret' field to 0, so the receiving node
			// knows that it has to generate an echo packet
			hdr->ret = 0;
			hdr->seq = seq++;
			// Store the current time in the 'send_time' field
			hdr->send_time = Scheduler::instance().clock();
			// Send the packet
			send(pkt, 0);
			// return TCL_OK, so the calling function knows that
			// the command has been processed
			return (TCL_OK);

		}

		else if (strcmp(argv[1], "start-WL-brdcast") == 0) {
			Packet* pkt = allocpkt();

			hdr_ip* iph = HDR_IP(pkt);
			hdr_esdndht* ph = hdr_esdndht::access(pkt);

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




void esdndhtAgent::recv(Packet* pkt, Handler*)
{
	// Access the IP header for the received packet:
	hdr_ip* hdrip = hdr_ip::access(pkt);

	// Access the Ping header for the received packet:
	hdr_esdndht* hdr = hdr_esdndht::access(pkt);


	// check if in brdcast mode
	if ((u_int32_t)hdrip->daddr() == IP_BROADCAST) {
		if (hdr->ret == 0) {

			printf("Recv BRDCAST Ping REQ : at %d.%d from %d.%d\n", here_.addr_, here_.port_, hdrip->saddr(), hdrip->sport());
			Packet::free(pkt);

			// create reply
			Packet* pktret = allocpkt();

			hdr_esdndht* hdrret = hdr_esdndht::access(pktret);
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
// NEW ADDITION

	/*
eSDN
*/
	if (hdr->ret == 1 || hdr->ret == 0 || hdr->start == 1 || hdr->start == 0 ) {
		// Send an 'echo'. First save the old packet's send_time
		cout<<"Agent esdndht is called from "<< dst_.addr_ << " to " <<  here_.addr_<< " for link "<<hdr->link_num << " with status "<< hdr->start <<endl;

		int link = hdr->link_num;

		double stime = hdr->send_time;
		int rcv_seq = hdr->seq;

		Node *me = Node::get_node_by_address(addr());

        if (me->poll_stat==0)
        {
            me->poll_stat = 1;
            Event* eve;
            me->expire(eve);
        }

		int from = dst_.addr_; // CHECK THIS



		for (int i=0;i<me->fixed_mappings.size();i++){
			if (me->fixed_mappings[i].address == addr()){
        //cout<<"addr()   "<<i<<"  "<<addr()<<endl;
        //cout<<"MATCHED !"<<endl;
        for (int j=0;j<me->fixed_mappings[i].fixed_links.size();j++){
					if (me->fixed_mappings[i].fixed_links[j].link_num == link ){
						for (int k=0;k<me->fixed_mappings[i].fixed_links[j].node_count.size();k++){
							if(me->fixed_mappings[i].fixed_links[j].node_count[k].node==from){
								if (hdr->start == 1)
								{
                                    //cout<<"INCREASING THE POLLING   for   "<< link <<"for node   "<<from<<endl;
                                    me->fixed_mappings[i].fixed_links[j].node_count[k].count++;
                                    //cout<<" IN esdndht  "<<me->fixed_mappings[i].fixed_links[j].node_count[k].count<<endl;
                                    Packet::free(pkt);
									return;
								}
								else if(hdr->start == 0 ){
                                    //cout<<"DECREASING NOW for "<< here_.addr_<<"  " <<link<<endl;
									me->fixed_mappings[i].fixed_links[j].node_count[k].count--;
									//cout<<" IN esdndht  "<<me->fixed_mappings[i].fixed_links[j].node_count[k].count<<endl;
                                    Packet::free(pkt);
                                    return;
								}
							}
						}
						if (hdr->start == 1)
						{
                            //cout<<"adding links "<<endl;
                            //cout<<"PUSHING THE LINK "<< link <<" FOR "<<from<<endl;
							Node_count nc;
							nc.node = from;
							nc.count = 1;
							me->fixed_mappings[i].fixed_links[j].node_count.push_back(nc);

                            Packet::free(pkt);
							return;
						}
						else if (hdr->start == 0)
						{
							printf("ERROR IN HDR->RET = 0 DHT KILL ME \n");
                            Packet::free(pkt);
							return;
						}
					}
				}
			}
		}
    Packet::free(pkt);

		// KILL ME NOW
	} else if(hdr->ret == 2 || hdr->start==2) {
        //    cout<<"Agent esdndht is called from "<<dst_.addr_ << " to " <<  here_.addr_<< " for link "<<hdr->link_num << " with status "<< hdr->no_flows <<endl;

        //cout<<"NUMBER OF FLOWS ON LINK   "<<hdr->link_num<<" is  "<<hdr->no_flows<<endl;

		Node* me = Node::get_node_by_address(addr());
		if (hdr->statDemanded == NUMFLOWS)
		 me->topo_links[hdr->link_num].stats.setStats_Num(hdr->utilization,hdr->q_lim,hdr->no_flows);
		else if (hdr->statDemanded == QLEN)
		 me->topo_links[hdr->link_num].stats.setStats_Queue(hdr->utilization,hdr->q_lim,hdr->q_len);


		Packet::free(pkt);
		return;
	}
    else
    {
        // Is the 'ret' field = 0 (i.e. the receiving node is being pinged)?
		// A packet was received. Use tcl.eval to call the Tcl
		// interpreter with the ping results.
		// Note: In the Tcl code, a procedure
		// 'Agent/Ping recv {from rtt}' has to be defined which
		// allows the user to react to the ping result.
		char out[100];
		// Prepare the output to the Tcl interpreter. Calculate the
		// round trip time
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
