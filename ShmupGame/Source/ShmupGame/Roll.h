#ifndef ROLL_H
#define ROLL_H

#include "Actor.h"

#include "Math/Vector.h"

#include <array>
#include <memory>

class AGameManager;
class Field;
class Ship;

class RollInitializer : public ActorInitializer {
public:
    RollInitializer(std::shared_ptr<Field> field, std::shared_ptr<Ship> ship, TWeakObjectPtr<AGameManager> gameManager) :
        m_field(field),
        m_ship(ship),
        m_gameManager(gameManager) {
    }

public:
    std::shared_ptr<Field> m_field;
    std::shared_ptr<Ship> m_ship;
    TWeakObjectPtr<AGameManager> m_gameManager;
};

class Roll : public Actor {
public:
    std::shared_ptr<Actor> newActor();

    void init(std::shared_ptr<ActorInitializer> initializer);
    void set();
    void remove();
    void tick();

public:
    enum {
        LENGTH = 4,
        NO_COLLISION_COUNT = 45
    };

    bool m_released;
    std::array<FVector2D, LENGTH> m_position;
    int m_cnt;

private:
    static const float BASE_LENGTH;
    static const float BASE_RESISTANCE;
    static const float BASE_SPRING;
    static const float BASE_SIZE;
    static const float BASE_DISTANCE;
    static const float SPEED;
    
    std::array<FVector2D, LENGTH> m_velocity;
    std::shared_ptr<Ship> m_ship;
    std::shared_ptr<Field> m_field;
    TWeakObjectPtr<AGameManager> m_gameManager;

    float m_distance;
};

#endif