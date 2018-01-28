#ifndef ENEMYINITIALIZER_H
#define ENEMYINITIALIZER_H

#include "ActorInitializer.h"

#include <memory>

class ActorPool;
class AGameManager;
class BulletActorPool;
class Roll;
class Lock;
//class Ship;

class EnemyInitializer : public ActorInitializer {
public:
    EnemyInitializer(std::shared_ptr<BulletActorPool> bullets,
        //std::shared_ptr<ActorPool> shots,
        std::shared_ptr<ActorPool> rolls,
        std::shared_ptr<ActorPool> locks,
        //std::shared_ptr<Ship> ship,
        AGameManager *gameManager) :
        m_bullets(bullets),
        //m_shots(shots),
        m_rolls(rolls),
        m_locks(locks),
        //m_ship(ship),
        m_gameManager(gameManager) {
    }

public:
    std::shared_ptr<BulletActorPool> m_bullets;
    //std::shared_ptr<Ship> m_ship;
    //std::shared_ptr<ActorPool> m_shots;
    std::shared_ptr<ActorPool> m_rolls;
    std::shared_ptr<ActorPool> m_locks;
    AGameManager *m_gameManager; //TSharedPtr<AGameManager>;
};

#endif