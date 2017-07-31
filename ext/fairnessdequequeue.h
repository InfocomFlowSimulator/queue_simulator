#ifndef FAIRNESS_DEQUE_QUEUE_H
#define FAIRNESS_DEQUE_QUEUE_H

#include "../coresim/queue.h"
#include "../coresim/packet.h"

#define PFABRIC_QUEUE 2

class FairnessDequeQueue : public Queue {
    public:
        FairnessDequeQueue(uint32_t id, double rate, uint32_t limit_bytes, int location);
        void enque(Packet *packet);
        Packet *deque();

        std::vector<std::vector<Packet*>> packetList;

        std::vector<Packet*> ackPackets;
};

#endif
