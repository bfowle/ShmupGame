#ifndef ROLL_H
#define ROLL_H

#include "Actor.h"

#include "Math/Vector.h"

#include <array>
#include <memory>

class AGameManager;
//class Ship;

class Roll : public Actor {
public:
    std::shared_ptr<Actor> newActor();

    void init(std::shared_ptr<ActorInitializer> initializer);
    void set();
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
    //std::shared_ptr<Ship> m_ship;
    //std::shared_ptr<Field> m_field;
    AGameManager *m_gameManager;

    float m_distance;
};

#endif