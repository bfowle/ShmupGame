#ifndef SHIP_H
#define SHIP_H

#include "Random.h"

#include "Math/Vector.h"

#include <memory>

class APawn;
class AGameManager;
class Field;

class Ship {
public:
    void init(std::shared_ptr<Field> field, AGameManager *gameManager);
    void start();
    void setPlayerPawn(APawn *playerPawn) { m_playerPawn = playerPawn; };
    void setSpeedRate(float rate);
    void destroyed();
    void tick();

public:
    enum {
        RESTART_COUNT = 300,
        INVINCIBLE_COUNT = 228
    };

    static bool m_isSlow;
    static const float SIZE;

    FVector2D m_position;
    bool m_restart;
    int m_cnt;

private:
    enum {
        BANK_BASE = 50
    };

    static Random m_random;
    static const float BASE_SPEED;
    static const float SLOW_BASE_SPEED;
    static const float FIRE_WIDE_BASE_DIRECTION;
    static const float FIRE_NARROW_BASE_DIRECTION;
    static const float TURRET_INTERVAL_LENGTH;
    static const float FIELD_SPACE;

    APawn *m_playerPawn;
    std::shared_ptr<Field> m_field;
    AGameManager *m_gameManager;
    FVector2D m_prevPosition;
    FVector2D m_velocity;
    FVector2D m_firePosition;

    float m_baseSpeed, m_slowSpeed;
    float m_speed;
    float m_bank;
    float m_fireWideDirection;
    int m_fireCnt;
    int m_ttlCnt;
    float m_fieldLimitX, m_fieldLimitY;
    int m_rollLockCnt;
    bool m_rollCharged;
};

#endif