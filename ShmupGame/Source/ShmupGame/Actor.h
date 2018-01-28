#ifndef ACTOR_H
#define ACTOR_H

#include "ActorInitializer.h"

#include <memory>

class Actor {
public:
    virtual ~Actor() {};
    virtual std::shared_ptr<Actor> newActor() = 0;
    virtual void init(std::shared_ptr<ActorInitializer> initializer) = 0;
    virtual void tick() = 0;

public:
    bool m_exists;
};

#endif