#include "fairnessdequequeue.h"
#include "../run/params.h"

#include <iostream>
#include <limits.h>
#include <algorithm>
#include <assert.h>

extern double get_current_time();
extern void add_to_event_queue(Event *ev);
extern DCExpParams params;

/* Fairness Queue */
FairnessDequeQueue::FairnessDequeQueue(uint32_t id, double rate, uint32_t limit_bytes, int location)
    : Queue(id, rate, limit_bytes, location) {}


void FairnessDequeQueue::enque(Packet *packet) {
    p_arrivals += 1;
    b_arrivals += packet->size;
    bytes_in_queue += packet->size;
    packet->last_enque_time = get_current_time();
    if (packet->type == NORMAL_PACKET)
    {
        const auto& iter = std::find_if(packetList.begin(), packetList.end(), [&packet](const auto& layer)->bool {return layer.front()->flow->id == packet->flow->id;});
        if (iter == packetList.end())
        {
            packetList.emplace_back();
            packetList.back().push_back(packet);
        }
        else
        {
            iter->push_back(packet);
        }
    }
    else
    {
        ackPackets.push_back(packet);
    }


    if (bytes_in_queue > limit_bytes) {
        if (packetList.size() == 0)
        {
            assert(ackPackets.size() > 0);
            pkt_drop++;
            bytes_in_queue -=ackPackets.back()->size;
            drop(ackPackets.back());
            ackPackets.pop_back();
            return;
        }
        int bad = 0;
        for (int i = 0; i < packetList.size(); i++)
        {
            if (packetList[i].size() >= packetList[bad].size())
            {
                bad = i;
            }
        }
        pkt_drop++;
        bytes_in_queue -= packetList[bad].back()->size;
        drop(packetList[bad].back());
        packetList[bad].pop_back();
        if (packetList[bad].size() == 0)
        {
            packetList.erase(packetList.begin() + bad);
        }
    }
}

Packet* FairnessDequeQueue::deque() {
    //std::cerr<<"hi in\n";
    if (bytes_in_queue > 0) {
        Packet* p;
        if (ackPackets.size() > 0)
        {
            p = ackPackets.front();
            ackPackets.erase(ackPackets.begin());
        }
        else
        {
            p = packetList.front().front();
            packetList.front().erase(packetList.front().begin());
            auto a = packetList[0];
            packetList.erase(packetList.begin());
            if (a.size() > 0)
            {
                 packetList.push_back(a);
            }
        }

        p_departures += 1;
        b_departures += p->size;
        bytes_in_queue -= p->size;
        p->total_queuing_delay += get_current_time() - p->last_enque_time;

        if(p->type ==  NORMAL_PACKET){
            if(p->flow->first_byte_send_time < 0)
                p->flow->first_byte_send_time = get_current_time();
            if(this->location == 0)
                p->flow->first_hop_departure++;
            if(this->location == 3)
                p->flow->last_hop_departure++;
        }
       // std::cerr<<"hi out\n" << "\n";
        return p;

    } else {
       // std::cerr<<"hi out\n" << "\n";
        return NULL;
    }

}

