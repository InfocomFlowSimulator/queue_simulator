#ifndef FAIRNESS_QUEUE_H
#define FAIRNESS_QUEUE_H

#include "../coresim/queue.h"
#include "../coresim/packet.h"

class PQ_Queue : public Queue {
    public:
        PQ_Queue(uint32_t id, double rate, uint32_t limit_bytes, int location);
        void enque(Packet *packet);
        Packet *deque();
        bool validate_duplicates(Packet* packet);
};

#endif
