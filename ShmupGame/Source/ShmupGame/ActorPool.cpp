#include "ActorPool.h"

using namespace std;

ActorPool::ActorPool(int n, Actor *actor, shared_ptr<ActorInitializer> initializer) :
    m_actorIdx(n) {
    m_actor.resize(n);

    for (int i = 0; i < m_actor.size(); ++i) {
        m_actor[i] = actor->newActor();
        m_actor[i]->m_exists = true;
        m_actor[i]->init(initializer);
    }
}

shared_ptr<Actor> ActorPool::getInstance() {
    for (int i = 0; i < m_actor.size(); ++i) {
        --m_actorIdx;
        if (m_actorIdx < 0) {
            m_actorIdx = m_actor.size() - 1;
        }
        if (!m_actor[m_actorIdx]->m_exists) {
            return m_actor[m_actorIdx];
        }
    }

    shared_ptr<Actor> null;
    return null;
}

shared_ptr<Actor> ActorPool::getInstanceForced() {
    --m_actorIdx;
    if (m_actorIdx < 0) {
        m_actorIdx = m_actor.size() - 1;
    }
    return m_actor[m_actorIdx];
}

void ActorPool::tick() {
    for (int i = 0; i < m_actor.size(); ++i) {
        if (m_actor[i]->m_exists) {
            m_actor[i]->tick();
        }
    }
}

void ActorPool::clear() {
    for (int i = 0; i < m_actor.size(); ++i) {
        m_actor[i]->m_exists = false;
    }
}