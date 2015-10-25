/* -*- Mode:C++; c-basic-offset:8; tab-width:8; indent-tabs-mode:t
              -*- */

/*
 * Copyright (C) 1997 by the University of Southern California
 * $Id: classifier-mpath.cc,v 1.10 2005/08/25 18:58:01 johnh Exp $
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

#ifndef lint
static const char rcsid[] = "@(#) $Header: /cvsroot/nsnam/ns-2/classifier/classifier-mpath.cc,v 1.10 2005/08/25 18:58:01 johnh Exp $ (USC/ISI)";
#endif

#include "classifier.h"
#include "ip.h"
#include "node.h"

/**
 *  The function of a node when it receives a packet is to examine the packet's fields,
 *  usually its destination address, and on occasion, its source address.
 *  It should then map the values to an outgoing interface object that is the next downstream
 *  recipient of this packet.
 *  The function of a node when it receives a packet is to examine the packet's fields,
 *  usually its destination address, and on occasion, its source address.
 *  It should then map the values to an outgoing interface object that is the next downstream recipient of this packet.
 */


/**
 * This object is devised to support equal cost multipath forwarding,
 * where the node has multiple equal cost routes to the same destination,
 * and would like to use all of them simultaneously.
 * This object does not look at any field in the packet.
 * With every succeeding packet, it simply returns the next filled slot in round robin fashion.
 *
 * Example round robin routing::
 *
 * class MultiPathForwarder : public Classifier {
 *    public:
 *        MultiPathForwarder() : ns_(0), Classifier() {}
 *        virtual int classify(Packet* const) {
 *                int cl;
 *                int fail = ns_;
 *                do {
 *                        cl = ns_++;
 *                        ns_ %= (maxslot_ + 1);
 *                } while (slot_[cl] == 0 && ns_ != fail);
 *                return cl;
 *        }
 *    private:
 *        int ns_;     // next slot to be used.  Probably a misnomer?
 *    };
 */
class MultiPathForwarder : public Classifier {
    public:
        MultiPathForwarder() : ns_(0), nodeid_(0), nodetype_(0), perflow_(0), checkpathid_(0) {
            bind("ns_", &ns_);
            bind("nodeid_", &nodeid_);
            bind("nodetype_", &nodetype_);
            bind("perflow_", &perflow_);
            bind("checkpathid_", &checkpathid_);
        }

        /*
         * This function returns the interger value which represents
         * the next link index to which the packet should be routed.
         */
        virtual int classify(Packet* p) {
            // Implement custom multipath routing algorithm here
            int cl;

            // Access packet IP headers to get its information
            hdr_ip* h = hdr_ip::access(p);

            // print packet headers in stdout
            // fprintf(stdout, "perflow_ = %d, rcv packet in classifier\n", perflow_);
            // fprintf(stdout, "checkpathid_ = %d \n", checkpathid_);
            // fprintf(stdout, "flow id:: %d \n", h->flowid());

            if (perflow_ || checkpathid_) {
                // define buffer
                struct hkey {
                    int nodeid;
                    nsaddr_t src, dst;
                    int fid;
                };

                // make buffer and put packet values inside it
                struct hkey buf_;
                buf_.nodeid = nodeid_;
                buf_.src = mshift(h->saddr());
                buf_.dst = mshift(h->daddr());
                buf_.fid = h->flowid();

                // convert buffer to string
                char* bufString = (char*) &buf_;
                int length = sizeof(hkey);

                // get buffers index value for the hashtable
                unsigned int ms_ = (unsigned int) HashString(bufString, length);

                if (checkpathid_) {
                    // checkpath id is enabled
                    // int pathNum = h->prio();
                    // int pathDig;
                    // for (int i = 0; i < nodetype_; i++) {
                    //     pathDig = pathNum % 8; // counts the numeber of times pathNum divided
                    //     pathNum /= 8;          // divide pathNum by 8 every iteration
                    // }
                    // printf("nodetype::%d: prio::%d\n", nodetype_, h->prio());
                    // add to the hashIndex the priority value of packet
                    ms_ += h->prio();
                }

                // mod to maxslot value
                ms_ %= (maxslot_ + 1);

                // printf("nodeid = %d, pri = %d, ms = %d\n", nodeid_, buf_.prio, ms_);
                int fail = ms_;
                // // do untill next route is found
                do {
                    cl = ms_++;
                    ms_ %= (maxslot_ + 1);
                } while (slot_[cl] == 0 && ms_ != fail);
                // Do log the calculated value
                // printf("nodeid = %d, pk_prio = %d, next_slot = %d\n", nodeid_, h->prio(), cl);
            } else {
                // if perlow and checkpathid_ are not set do simple round robin routing
                int fail = ns_;
                do {
                    cl = ns_++;
                    ns_ %= (maxslot_ + 1);
                } while (slot_[cl] == 0 && ns_ != fail);
            }

            // return next slot value
            //printf("returning %d\n", cl);
            // TODO FIX THIS
            return cl;
            //printf("returning 1\n");
        }

    private:
        int ns_;
        // Mohamamd: adding support for perflow multipath
        int nodeid_;      // holds the nodeID
        int nodetype_;    // holds the node type
        int perflow_;
        int checkpathid_; // consider the packet prio when computing the next route

        /*
         * Calculates the hashtables index of the given string.
         */
        static unsigned int HashString(register const char *bytes,int length)
        {
            register unsigned int result;
            register int i;

            result = 0;
            for (i = 0;  i < length;  i++) {
                result += (result<<3) + *bytes++;
            }
            return result;
        }
};

static class MultiPathClass : public TclClass {
    public:
        MultiPathClass() : TclClass("Classifier/MultiPath") {}
        TclObject* create(int, const char*const*) {
            return (new MultiPathForwarder());
        }
} class_multipath;
