/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- 
 *
 * Copyright (c) 1997-2000 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the Computer Systems
 *	Engineering Group at Lawrence Berkeley Laboratory.
 * 4. Neither the name of the University nor of the Laboratory may be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Header: /cvsroot/nsnam/ns-2/common/node.h,v 1.33 2002/05/30 17:44:03 haldar Exp $
 */

/*
 * XXX GUIDELINE TO ADDING FUNCTIONALITY TO THE BASE NODE
 *
 * One should not add something specific to one particular routing module 
 * into the base node, which is shared by all modules in ns. Otherwise you 
 * bloat other people's simulations.
 */

/*
 * CMU-Monarch project's Mobility extensions ported by Padma Haldar, 
 * 10/98.
 */
 
#ifndef __ns_node_h__
#define __ns_node_h__

#include "connector.h"
#include "object.h"
#include "lib/bsd-list.h"

#include "phy.h"
#include "net-interface.h"
#include "energy-model.h"
#include "location.h"
#include "rtmodule.h"

#define POLL_RATE 0.0003 // only useful in case of information sharing. 

class NixNode;
class LinkHead;

LIST_HEAD(linklist_head, LinkHead); // declare list head structure 
/*
 * The interface between a network stack and the higher layers.
 * This is analogous to the OTcl Link class's "head_" object.
 * XXX This object should provide a uniform API across multiple link objects;
 * right now it is a placeholder.  See satlink.h for now.  It is declared in
 * node.h for now since all nodes have a linked list of LinkHeads.
 */
//#include <iostream>

#include <fstream>
#include <string>
#include <vector>
#include "parentnode.h"
using namespace std;
class Node;
class NetworkInterface;
class RoutingModule;

// not being used. 
struct Total_count{
	int count;
};
extern Total_count* t_c;
class LinkHead : public Connector {
public:
	LinkHead(); 

	// API goes here
	Node* node() { return node_; }
	int type() { return type_; }
	int32_t label();
	// Future API items could go here 

	// list of all networkinterfaces on a node
	inline void insertlink(struct linklist_head *head) {
                LIST_INSERT_HEAD(head, this, link_entry_);
        }
        LinkHead* nextlinkhead(void) const { return link_entry_.le_next; }

protected:
	virtual int command(int argc, const char*const* argv);
	NetworkInterface* net_if_; // Each link has a Network Interface
	Node* node_; // Pointer to node object
	int type_; // Type of link
	// Each node has a linked list of link heads
	LIST_ENTRY(LinkHead) link_entry_;

};

LIST_HEAD(node_head, Node); // declare list head structure 

//declare the neighbor list structure
//added for pushback, but should be useful otherwise also.
//there was something here which was moved to energy model. 
//since that work is in flux, i am not meddling with it.
struct neighbor_list_node {
	int nodeid;
	neighbor_list_node* next;
};

// Size of the buffer for dumping nam traces.
const int NODE_NAMLOG_BUFSZ = 256;

//routing module node used for creating rtg module list
struct rtm_node {
	RoutingModule* rtm;
	rtm_node* next;
};

//eSDN
struct Topo_nodes{
	int node_num;
};
// eSDN

struct Topo_stats {

	double utilization;
	double q_lim;
	int no_flows;
	double q_size;
	void setStats(double util,double qL,int nF,double qS) {
		utilization = util;
		q_lim = qL;
		no_flows = nF;
		q_size = qS;
	}
	void setStats_Num(double util,double qL,int nF){
		utilization = util;
		q_lim = qL;
		no_flows = nF;		
	}
	void setStats_Queue(double util,double qL,double qS){
		utilization = util;
		q_lim = qL;
		q_size = qS;		
	}
};
//eSDN. every node has every link in the toplogy in a vector. 
struct Topo_links{
	int n_start;
	int n_end;
	int link_num;
	Topo_stats stats;
	double last_poll; // would tell the expiry date for stats. 
	double capacity; // static. read from text file. 
	// called on intilization of each link. from node.cc file. which reads info from file. 
	Topo_links(int s,int e,int n,double c){
		n_start = s;
		n_end = e;
		capacity = c;
		link_num = n;
		last_poll = -1;
		stats.setStats(0,0,0,0);
	}
};

// eSDN
// at count 0, the responsible guy for a given link stops sending me data. 
struct Node_count { // each of this object identified by 2-tupele. 
	int node; // ID. 
	int count;
	//int stat_demand; // 2 = link count; 2 = port stats, 0 = both
};

struct Fixed_links{
	int link_num;
	vector<Node_count> node_count;
	Fixed_links(int a){
		link_num = a;
	}
};

// working in node.cc
// each node has a list of these mappings. used to find the host responsible for a link. 
struct Fixed_mappings {
	int address;
	vector<Fixed_links> fixed_links;
};

class Node : public ParentNode, public TimerHandler {
public:
	Node(void);
	~Node();
	void send_link(int,int);
	void poll_link(int,int);
	void send_stats();
	void fetch_stats();
	virtual void expire (Event *e);
	inline int address() { return address_;}
	inline int nodeid() { return nodeid_;}
	inline bool exist_namchan() const { return (namChan_ != 0); }

	virtual int command(int argc, const char*const* argv);
	virtual void namlog(const char *fmt, ...);

	NsObject* intf_to_target(int32_t); 

	static struct node_head nodehead_;  // static head of list of nodes
	inline void insert(struct node_head* head) {
		LIST_INSERT_HEAD(head, this, entry);
	}
	inline Node* nextnode() { return entry.le_next; }

	// The remaining objects handle a (static) linked list of nodes
	// Used by Tom's satallite code and the wireless code
	inline const struct if_head& ifhead() const { return ifhead_; }
	inline const struct linklist_head& linklisthead() const { 
		return linklisthead_; 
	}
	
	//neighbor list maintenance
	neighbor_list_node* neighbor_list_;
	void addNeighbor(Node *node);
	
	static Node* get_node_by_address(nsaddr_t);
	
	//routines for supporting routing
	void route_notify (RoutingModule *rtm);
	void unreg_route_notify(RoutingModule *rtm);
	void add_route (char *dst, NsObject *target);
	void delete_route (char *dst, NsObject *nullagent);
	void set_table_size(int nn);
	void set_table_size(int level, int csize);

public:		// protected changed by RAZ
	LIST_ENTRY(Node) entry;  // declare list entry structure
	int address_;
	int nodeid_; 		 // for nam use
	int poll_stat;
	int addr(){return address_;}
	vector<Topo_nodes> topo_nodes; // eSDN. 
	void print_stats();
	vector<Topo_links> topo_links; // eSDN
	vector<Fixed_mappings> fixed_mappings; // eSDN
	// Nam tracing facility
        Tcl_Channel namChan_;
	// Single thread ns, so we can use one global storage for all 
	// node objects
	static char nwrk_[NODE_NAMLOG_BUFSZ];	
	void namdump();

	struct if_head ifhead_; // list of phys for this node
	struct linklist_head linklisthead_; // list of link heads on node

	// pointer to head of rtmodule chain
	RoutingModule* rtnotif_;
	int num_flow;	//RAZA FRIDAY
#ifdef HAVE_STL
	NixNode* nixnode_;   // used for nix routing (on-demand source routing for simulator performance)
#endif /* STL */

public:
	// XXX Energy related stuff. Should be moved later to a wireless 
	// routing plugin module instead of sitting here.
	inline EnergyModel* energy_model() { return energy_model_; }
	inline Location* location() { return location_; }
protected:
	EnergyModel* energy_model_;
	// XXX Currently this is a placeholder only. It is supposed to 
	// hold the position-related stuff in MobileNode. Yet another 
	// 'Under Construction' sign :(
	Location* location_;
};

#endif /* __ns_node_h__ */
