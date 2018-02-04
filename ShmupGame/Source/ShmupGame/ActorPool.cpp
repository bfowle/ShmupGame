#include "ActorPool.h"

using namespace std;

ActorPool::ActorPool(int size, Actor *actor, shared_ptr<ActorInitializer> initializer) :
    m_actorIdx(size) {
    m_pool.resize(size);

    for (int i = 0; i < m_pool.size(); ++i) {
        m_pool[i] = actor->newActor();
        m_pool[i]->m_isAlive = true;
        m_pool[i]->init(initializer);
    }
}

shared_ptr<Actor> ActorPool::getInstance() {
    for (int i = 0; i < m_pool.size(); ++i) {
        --m_actorIdx;
        if (m_actorIdx < 0) {
            m_actorIdx = m_pool.size() - 1;
        }
        if (!m_pool[m_actorIdx]->m_isAlive) {
            return m_pool[m_actorIdx];
        }
    }

    shared_ptr<Actor> null;
    return null;
}

shared_ptr<Actor> ActorPool::getInstanceForced() {
    --m_actorIdx;
    if (m_actorIdx < 0) {
        m_actorIdx = m_pool.size() - 1;
    }
    return m_pool[m_actorIdx];
}

void ActorPool::tick() {
    for (int i = 0; i < m_pool.size(); ++i) {
        if (m_pool[i]->m_isAlive) {
            m_pool[i]->tick();
        }
    }
}

void ActorPool::clear() {
    for (int i = 0; i < m_pool.size(); ++i) {
        m_pool[i]->m_isAlive = false;
    }
}