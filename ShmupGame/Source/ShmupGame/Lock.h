#ifndef LOCK_H
#define LOCK_H

#include "Actor.h"
#include "Random.h"

#include "Math/Vector.h"

#include <array>
#include <memory>

class AGameManager;
class Enemy;
class Field;
class Ship;

class LockInitializer : public ActorInitializer {
public:
    LockInitializer(std::shared_ptr<Field> field, std::shared_ptr<Ship> ship, TWeakObjectPtr<AGameManager> gameManager) :
        m_field(field),
        m_ship(ship),
        m_gameManager(gameManager) {
    }

public:
    std::shared_ptr<Field> m_field;
    std::shared_ptr<Ship> m_ship;
    TWeakObjectPtr<AGameManager> m_gameManager;
};

class Lock : public Actor {
public:
    Lock() {};

    std::shared_ptr<Actor> newActor();

    static void init();
    void init(std::shared_ptr<ActorInitializer> initializer);
    void set();
    void hit();
    void remove();
    void tick();

private:
    void reset();

public:
    enum {
        SEARCH,
        SEARCHED,
        LOCKING,
        LOCKED,
        FIRED,
        HIT,
        CANCELED
    };

    enum {
        LENGTH = 12,
        NO_COLLISION_COUNT = 8
    };

    int m_state;
    std::array<FVector2D, LENGTH> m_position;
    int m_cnt;
    float m_lockMinY;
    std::shared_ptr<Enemy> m_lockedEnemy;
    int m_lockedPart;
    FVector2D m_lockedPosition;
    bool m_released;

private:
    enum {
        LOCK_CNT = 8
    };
    
    static const float SPEED;

    static Random m_random;
    FVector2D m_velocity;
    std::shared_ptr<Ship> m_ship;
    std::shared_ptr<Field> m_field;
    TWeakObjectPtr<AGameManager> m_gameManager;
};

#endif