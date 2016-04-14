	/* -*-	Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t -*- */
	/*
	 * Copyright (c) 1996-1997 The Regents of the University of California.
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
	 * 	This product includes software developed by the Network Research
	 * 	Group at Lawrence Berkeley National Laboratory.
	 * 4. Neither the name of the University nor of the Labo ratory may be used
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
	 */

	#ifndef lint
	 static const char rcsid[] =
	 "@(#) $Header: /cvsroot/nsnam/ns-2/queue/queue.cc,v 1.29 2004/10/28 01:22:48 sfloyd Exp $ (LBL)";
	#endif

	#include "queue.h"
	#include <math.h>
	#include <stdio.h>
	#include <iostream>



	 bool compare_strings(char* x, char *y) {
	 	for (int i =0; i < 67; i++) {
	 		if (x[i] != y[i]) {
	 			return false;
	 		}
	 	}
	 	return true;
	 }
	// Converts teh int to char and concats
	 char* concat(int src, int src_prt, int dst, int dst_prt)
	 {
	 	char* arr = new char[67];
	// init array
	 	for (int i = 0; i < 67; ++i)
	 	{
	 		arr[i] = 0;
	 	}
	// put int
	 	arr[0] = src;
	 	arr[16] = src_prt;
	 	arr[33] = dst;
	 	arr[50] = dst_prt;
	 	return arr;
	 }

	 globalPQ* pq_head = NULL;
	 globalQ* q_head = NULL;
	 pathway* pathway_global = NULL;




	 Queue* globalQueue1 = NULL;

	 GlobalQueue* global_q_head = NULL;

	 void Queue::printStats() {
	 	printf("Queue Size = ");
	 	printf("%d\n", pq_->bytes_   );
	 	printf("\n PACKET COUNT = ");
	 	printf("%d\n", pq_->len_);
	 	printf("\n Utilization is : ");
	 	printf("%f\n", cur_util_ );

	 }


	 int Queue::num_flows(){
		//printf("%d",numFlows);
	 	return numFlows;
	 }

	 PacketQueue::PacketQueue() : head_(0), tail_(0), len_(0), bytes_(0)
	 {
	// Add packet to linked list
	// custom code
	 	globalPQ* temp = new globalPQ;
	 	temp->packet_queue_pointer = this;
	 	temp->next = NULL;
	 	if (pq_head == NULL)
	// define the LL head
	 		pq_head = temp;
	 	else {
	// insert at end
	 		globalPQ* temp1 = pq_head;
	 		while(temp1->next!=NULL){
	 			temp1=temp1->next;
	 		}
	 		temp1->next=temp;
	 	}
	 }

Packet* PacketQueue::enque(Packet* p)
{
    // if (p != NULL)
    // {
    //     hdr_cmn* hdr_cmn = HDR_CMN(p);
    //     printf("pkt uid: ");
    //     printf(UID_PRINTF_FORMAT, hdr_cmn->uid());
    //     printf(" | ");
    //     printf("type: %d", hdr_cmn->ptype());
    //     printf("\n");
    // }

	// Returns previous tail
    // custom code
    // Traverse the linked list and print the packet which matches
    // point to this packet_queue
    // print packet info everytime its enqueued
    if (pq_head) {
        globalPQ* tmp = pq_head;
        int counter = 0;
        while (tmp) {
            if (p) {
                if (this == tmp->packet_queue_pointer) {

                    // get queue limit for the given packet queue
                    globalQ* tmp_q = q_head;
                    for (int i = 0; i < counter; ++i) {
                        tmp_q = tmp_q->next;
                    }

                    // counter value is link number
                    hdr_ip* h = hdr_ip::access(p);

                   // add to global dictobject
                    if(pathway_global) {
                        // pathway not null
                        // define key
                        char* key = concat(h->src_.addr_, h->src_.port_, h->dst_.addr_, h->dst_.port_);
						// printf("updating for key::\n");
      //                   printf("src::%d, src_prt:: %d,dst::%d,dst_prt::%d,",h->src_.addr_, h->src_.port_, h->dst_.addr_, h->dst_.port_);
      //                   printf("counter is::%d\n", counter);
      //                   printf("key::");
      //                   for (int i = 0; i < 67; ++i)
      //                   	printf("%d", key[i]);
      //                   printf("\n");

                        bool updated = false;

                        // check if in map content
                        // TODO:: change to O(1)
                        for (std::map<char*, pathway_flow*>::iterator it=pathway_global->flow_map.begin(); it!=pathway_global->flow_map.end(); ++it) {
                            if(compare_strings(it->first, key)) {
                                // if key found
                                // it->first = KEY
                                // it->second = VALUE
                                if(it->second != NULL) {
                                    pathway_link* head = it->second->links;
                                    // Do insertion in linked list
                                    // traverse list
                                    pathway_link* iter = head;
                                    pathway_link* tail = head;
                                    while (iter) {
                                        tail = iter;
                                        // skip if link_id already in list
                                        if(iter->link_id == counter) {
                                            updated = true;
                                            //printf("link_id already in list skipping.\n");
                                        }
                                        iter = iter->next;
                                    }
                                    if(!updated) {
                                        if (tail != NULL) {
                                            // append to tail if not updated yet
                                            tail->next = new pathway_link();
                                            tail->next->link_id = counter;
                                            tail->next->next = NULL;
                                            // print key
                                            // printf("updating for key::\n");
                                            // for (int i = 0; i < 67; ++i)
                                            //     printf("%d", key[i]);
                                            // printf("\n");
                                            // printf("added link_id::%d\n", counter);
                                        } else {
                                            printf("ERROR::queue::tail is null\n");
                                        }
                                    }
                                    // end insertion
                                } else {
                                    printf("ERROR::packetqueue::enqueue::error key's value is NULL\n");
                                }
                                // flag update has been done
                                updated = true;
                            }
                        }

                        if (!updated) {
                            // make flow
                            pathway_flow* pathway_flow_ = new pathway_flow();

                            // define the src dest and ports
                            pathway_flow_->src = h->src_.addr_;
                            pathway_flow_->src_prt = h->src_.port_;
                            pathway_flow_->dst = h->dst_.addr_;
                            pathway_flow_->dst_prt = h->dst_.port_;

                            // add link
                            pathway_link* l = new pathway_link();
                            l->link_id =counter;
                            l->next = NULL;
                            pathway_flow_->links = l;

                            // make key
                            pathway_global->flow_map.insert(std::pair<char*, pathway_flow*>(key, pathway_flow_));
                            // printf("updating for key::\n");
                            // for (int i = 0; i < 67; ++i)
                            //     printf("%d", key[i]);
                            // printf("\n");
                            // printf("added link_id::%d\n", counter);
                        }
                        // printf("==========================================\n");
                    } else {
                        //  if null
                        pathway_global = new pathway();
                        pathway_flow* pathway_flow_ = new pathway_flow();
                        // define the src dest and ports
                        pathway_flow_->src = h->src_.addr_;
                        pathway_flow_->src_prt = h->src_.port_;
                        pathway_flow_->dst = h->dst_.addr_;
                        pathway_flow_->dst_prt = h->dst_.port_;

                        // add link
                        pathway_link* l = new pathway_link();
                        l->link_id =counter;

                        pathway_flow_->links = l;

                        // make key
                        char* key = concat(h->src_.addr_, h->src_.port_, h->dst_.addr_, h->dst_.port_);
                        pathway_global->flow_map.insert(std::pair<char*, pathway_flow*>(key, pathway_flow_));
                        // printf("updating for key::\n");
                        // for (int i = 0; i < 67; ++i)
                        //     printf("%d", key[i]);
                        // printf("\n");
                        // printf("added link_id::%d\n", counter);
                    }
                }
            }
            counter += 1;
            tmp = tmp->next;
        }
    }
    // custom code end


    Packet* pt = tail_;
    if (!tail_) head_= tail_= p;
    else {
        tail_->next_= p;
        tail_= p;
    }
    tail_->next_= 0;
    ++len_;
    bytes_ += hdr_cmn::access(p)->size();
    return pt;
}


	int PacketQueue::num_flows(){
		Flow1* temp;
		while(flow_pointer != NULL){
			temp = flow_pointer->next;
			delete flow_pointer;
			flow_pointer = temp;
		}
		bool res = 0;
		for (Packet* p = head_; p != 0; p = p->next_){
			hdr_ip* ip = hdr_ip::access(p);
			hdr_cmn* hdr_cmn = HDR_CMN(p);

			//HP so that we dont count the ping packets as separate flows.
			// if (hdr_cmn->ptype() == 44) {
			// 	continue;
			// }
			//HP end

			Flow1* f1 = new Flow1(hdr_cmn->ptype());
			f1->src_addr_ = ip->src_.addr_;
			f1->src_port_ = ip->src_.port_;
			f1->dst_addr_ = ip->dst_.addr_;
			f1->dst_port_ = ip->dst_.port_;

			temp = flow_pointer;
			res = f1->equal(temp);
			if(res==0)
			{
				f1->next=flow_pointer;
				flow_pointer=f1;
			}
		}
		int count = 0;
		temp = flow_pointer;
		while ( temp!=NULL ){
			count++;
			// if (temp->getType() == 44) { // HP
			// 	count--;
			// }
			temp=temp->next;
		}
		printf("TOTAL FLOWS ARE : ");
		printf("%d\n", count);
		return count;
	}


	void PacketQueue::remove(Packet* target)
	{
		for (Packet *pp= 0, *p= head_; p; pp= p, p= p->next_) {
			if (p == target) {
				if (!pp) deque();
				else {
					if (p == tail_)
						tail_= pp;
					pp->next_= p->next_;
					--len_;
					bytes_ -= hdr_cmn::access(p)->size();
				}
				return;
			}
		}
		fprintf(stderr, "PacketQueue:: remove() couldn't find target\n");
		abort();
	}

	/*
	 * Remove packet pkt located after packet prev on the queue.  Either p or prev
	 * could be NULL.  If prev is NULL then pkt must be the head of the queue.
	 */
	void PacketQueue::remove(Packet* pkt, Packet *prev) //XXX: screwy
	{
		if (pkt) {
			if (head_ == pkt)
				PacketQueue::deque(); /* decrements len_ internally */
				else {
					prev->next_ = pkt->next_;
					if (tail_ == pkt)
						tail_ = prev;
					--len_;
					bytes_ -= hdr_cmn::access(pkt)->size();
				}
			}
			return;
		}

		void QueueHandler::handle(Event*)
		{
			queue_.resume();
		}

		Queue::~Queue() {
		}


		Queue::Queue() : Connector(), blocked_(0), unblock_on_resume_(1), qh_(*this),
		pq_(0),
			 last_change_(0), /* temporarily NULL */
		old_util_(0), period_begin_(0), cur_util_(0), buf_slot_(0),
		util_buf_(NULL)
		{


			bind("limit_", &qlim_);
			bind("util_weight_", &util_weight_);
			bind_bool("blocked_", &blocked_);
			bind_bool("unblock_on_resume_", &unblock_on_resume_);
			bind("util_check_intv_", &util_check_intv_);
			bind("util_records_", &util_records_);
			printf("1");
			if (util_records_ > 0) {
				util_buf_ = new double[util_records_];
				if (util_buf_ == NULL) {
					printf("Error allocating util_bufs!");
					util_records_ = 0;
				}
				for (int i = 0; i < util_records_; i++) {
					util_buf_[i] = 0;
				}
			}
			numFlows = 0;
		//flow_pointer = NULL;

			if(globalQueue1==NULL)
				globalQueue1 = this;

			GlobalQueue* temp = new GlobalQueue;
			temp->que = this;
			temp->next = NULL;

			if (global_q_head == NULL)
				global_q_head = temp;
			else {
				GlobalQueue* temp1 = global_q_head;
				while(temp1->next!=NULL){
					temp1=temp1->next;
				}
				temp1->next=temp;
			}



			globalQ* tempS = new globalQ;
			tempS->queue_pointer = this;
			tempS->next = NULL;
			if (q_head == NULL)
		// define the LL head
				q_head = tempS;
			else {
		// insert at end
				globalQ* temp1S = q_head;
				while(temp1S->next!=NULL){
					temp1S=temp1S->next;
				}
				temp1S->next=tempS;
			}




		}

		void Queue::recv(Packet* p, Handler*)
		{
			double now = Scheduler::instance().clock();
			enque(p);
			if (!blocked_) {
			/*
			 * We're not blocked.  Get a packet and send it on.
			 * We perform an extra check because the queue
			 * might drop the packet even if it was
			 * previously empty!  (e.g., RED can do this.)
			 */
			 p = deque();
			 if (p != 0) {
			 	utilUpdate(last_change_, now, blocked_);
				//printf("%d,")
			 	last_change_ = now;
			 	blocked_ = 1;
			 	target_->recv(p, &qh_);
			 }
			}
		}

		void Queue::utilUpdate(double int_begin, double int_end, int link_state) {
			double decay;

			decay = exp(-util_weight_ * (int_end - int_begin));
			old_util_ = link_state + (old_util_ - link_state) * decay;

		// PS: measuring peak utilization
			if (util_records_ == 0)
			return; // We don't track peak utilization

		double intv = int_end - int_begin;
		double tot_intv = int_begin - period_begin_;
		if (intv || tot_intv) {
			int guard = 0; // for protecting against long while loops
			cur_util_ = (link_state * intv + cur_util_ * tot_intv) /
			(intv + tot_intv);
			while (tot_intv + intv > util_check_intv_ &&
				guard++ < util_records_) {

				period_begin_ = int_end;
			util_buf_[buf_slot_] = cur_util_;
			buf_slot_ = (buf_slot_ + 1) % util_records_;
			cur_util_ = link_state;
			intv -= util_check_intv_;
		}
	}
	}

	double Queue::utilization(void)
	{
		double now = Scheduler::instance().clock();

		utilUpdate(last_change_, now, blocked_);
		last_change_ = now;

		return old_util_;

	}

	double Queue::peak_utilization(void)
	{
		double now = Scheduler::instance().clock();
		double peak = 0;
		int i;

		// PS: if peak_utilization tracking is disabled,
		// return the weighed avg instead
		if (util_records_ == 0)
			return utilization();

		utilUpdate(last_change_, now, blocked_);
		last_change_ = now;

		for (i = 0; i < util_records_; i++) {
			if (util_buf_[i] > peak)
				peak = util_buf_[i];
		}
		return peak;
	}

	void Queue::updateStats(int queuesize)
	{
		double now = Scheduler::instance().clock();
		double newtime = now - total_time_;
		if (newtime > 0.0) {
			double oldave = true_ave_;
			double oldtime = total_time_;
			double newtime = now - total_time_;
			true_ave_ = (oldtime * oldave + newtime * queuesize) /now;
			total_time_ = now;
		}
	}

	void Queue::resume()
	{
		double now = Scheduler::instance().clock();
		Packet* p = deque();
		if (p != 0) {
			target_->recv(p, &qh_);
		} else {
			if (unblock_on_resume_) {
				utilUpdate(last_change_, now, blocked_);
				last_change_ = now;
				blocked_ = 0;
			}
			else {
				utilUpdate(last_change_, now, blocked_);
				last_change_ = now;
				blocked_ = 1;
			}
		}
	}

	void Queue::reset()
	{
		Packet* p;
		total_time_ = 0.0;
		true_ave_ = 0.0;
		while ((p = deque()) != 0)
			drop(p);
	}

