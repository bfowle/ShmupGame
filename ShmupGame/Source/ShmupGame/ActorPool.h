#ifndef ACTORPOOL_H
#define ACTORPOOL_H

#include "Actor.h"
#include "ActorInitializer.h"

#include <algorithm>
#include <memory>
#include <vector>

class ActorPool {
public:
    ActorPool(int size, Actor *actor, std::shared_ptr<ActorInitializer> initializer);
    virtual ~ActorPool() {};

    std::shared_ptr<Actor> getInstance();
    std::shared_ptr<Actor> getInstanceForced();
    void tick();
    void clear();

public:
    std::vector<std::shared_ptr<Actor>> m_pool;
    int m_actorIdx;
};

#endif