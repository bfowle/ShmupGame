#ifndef ENEMYINITIALIZER_H
#define ENEMYINITIALIZER_H

#include "ActorInitializer.h"

#include <memory>

class ActorPool;
class AGameManager;
class BulletActorPool;
class Field;
class Ship;
class Roll;
class Lock;
class UWorld;

class EnemyInitializer : public ActorInitializer {
public:
    EnemyInitializer(std::shared_ptr<Field> field, std::shared_ptr<Ship> ship, std::shared_ptr<BulletActorPool> bullets, std::shared_ptr<ActorPool> rolls, std::shared_ptr<ActorPool> locks, /*std::shared_ptr<ActorPool> shots,*/ TWeakObjectPtr<AGameManager> gameManager) :
        m_field(field),
        m_ship(ship),
        m_bullets(bullets),
        //m_shots(shots),
        m_rolls(rolls),
        m_locks(locks),
        m_gameManager(gameManager) {
    }

public:
    std::shared_ptr<Field> m_field;
    std::shared_ptr<Ship> m_ship;
    std::shared_ptr<BulletActorPool> m_bullets;
    std::shared_ptr<ActorPool> m_rolls;
    std::shared_ptr<ActorPool> m_locks;
    //std::shared_ptr<ActorPool> m_shots;
    TWeakObjectPtr<AGameManager> m_gameManager;
};

#endif