#ifndef ACTOR_H
#define ACTOR_H

#include "HAL/Platform.h"

#include <memory>

class ActorInitializer {
public:
    virtual ~ActorInitializer() {};
};

class Actor {
public:
    virtual ~Actor() {};
    virtual std::shared_ptr<Actor> newActor() = 0;
    virtual void init(std::shared_ptr<ActorInitializer> initializer) = 0;
    virtual void remove() = 0;
    virtual void tick() = 0;

public:
    uint32 m_uuid;
    int32 m_instanceId;
    bool m_isAlive;
};

#endif