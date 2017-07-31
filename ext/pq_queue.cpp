#include "pq_queue.h"
#include "../run/params.h"

#include <iostream>
#include <limits.h>

extern double get_current_time();
extern void add_to_event_queue(Event *ev);
extern DCExpParams params;

/* Fairness Queue */
PQ_Queue::PQ_Queue(uint32_t id, double rate, uint32_t limit_bytes, int location)
    : Queue(id, rate, limit_bytes, location) {}

#define FAIRNESS_PRIORITY 10
#define FIFO_PRIOROTY 20
#define SRPT_PRIORITY 30

int compValues(uint32_t a, uint32_t b)
{
    if (a == b)
    {
        return 0;
    }
    return  a < b ? -1 : 1;
}

int compareByPriority(Packet* p1, Packet* p2)
{
    if (p1->type == NORMAL_PACKET && p2->type == ACK_PACKET)
    {
        return 1;
    }

    if (p1->type == ACK_PACKET)
    {
        return p2->type == NORMAL_PACKET ? -1 : 0;
    }

    if (params.pq_mode == FAIRNESS_PRIORITY)
    {
        return compValues(p1->seq_no,  p2->seq_no);
    }

    if (params.pq_mode == SRPT_PRIORITY)
    {
        return compValues(p1->pf_priority, p2->pf_priority);
    }

    return 0;
}

void PQ_Queue::enque(Packet *packet) {
    p_arrivals += 1;
    b_arrivals += packet->size;
    packets.push_back(packet);
    bytes_in_queue += packet->size;
    packet->last_enque_time = get_current_time();
    if (bytes_in_queue > limit_bytes) {
        uint32_t worst_index = 0;
        for (uint32_t i = 0; i < packets.size(); i++) {
            if (compareByPriority(packets[i], packets[worst_index]) >= 0) {
                worst_index = i;
            }
        }
        bytes_in_queue -= packets[worst_index]->size;
        Packet *worst_packet = packets[worst_index];

        packets.erase(packets.begin() + worst_index);
        pkt_drop++;
        drop(worst_packet);
    }
}

Packet* PQ_Queue::deque() {
    if (bytes_in_queue > 0) {

        uint32_t best_index = 0;
        for (uint32_t i = 0; i < packets.size(); i++) {
            Packet* curr_pkt = packets[i];
            if (compareByPriority(curr_pkt, packets[best_index]) < 0) {
                best_index = i;
            }
        }

        Packet *p = packets[best_index];
        bytes_in_queue -= p->size;
        packets.erase(packets.begin() + best_index);

        p_departures += 1;
        b_departures += p->size;

        p->total_queuing_delay += get_current_time() - p->last_enque_time;

        if(p->type ==  NORMAL_PACKET){
            if(p->flow->first_byte_send_time < 0)
                p->flow->first_byte_send_time = get_current_time();
            if(this->location == 0)
                p->flow->first_hop_departure++;
            if(this->location == 3)
                p->flow->last_hop_departure++;
        }
        return p;

    } else {
        return NULL;
    }
}

