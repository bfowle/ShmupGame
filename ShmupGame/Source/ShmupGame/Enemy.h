#ifndef ENEMY_H
#define ENEMY_H

#include "Actor.h"
#include "EnemyInitializer.h"
#include "EnemyType.h"
#include "Random.h"

#include "Math/Vector.h"

#include <array>
#include <memory>

class AGameManager;
class ActorPool;
class BulletActor;
class BulletActorPool;
class Lock;
class Ship;

class Enemy : public Actor, public std::enable_shared_from_this<Enemy> {
public:
    std::shared_ptr<Enemy> getPtr();
    std::shared_ptr<Actor> newActor();
    void init(std::shared_ptr<ActorInitializer> initializer);
    void set(const FVector2D &position, float direction, std::shared_ptr<EnemyType> type, BulletMLParser *parser);
    void setBoss(const FVector2D &p, float direction, std::shared_ptr<EnemyType> type);
    void tick();
    void tickBoss();

private:
    struct Battery {
        std::array<std::shared_ptr<BulletActor>, BatteryType::WING_BATTERY_MAX> m_topBullet;
        int m_shield;
        bool m_isDamaged;
    };

    std::shared_ptr<BulletActor> setBullet(const Barrage &barrage, const FVector2D *ofs, float xr);
    std::shared_ptr<BulletActor> setBullet(const Barrage &barrage, const FVector2D *ofs);
    void setTopBullets();
    void removeBattery(Battery *battery, const BatteryType &bt);
    void addDamageBattery(int idx, int dmg);
    //int checkHit(const FVector2D &position, float xofs, float yofs);
    int checkLocked(const FVector2D &position, float xofs, std::shared_ptr<Lock> lock);
    void removeTopBullets();
    void remove();
    void gotoNextPoint();
    void controlFireCnt();

public:
    FVector2D m_position;
    std::shared_ptr<EnemyType> m_type;
    std::array<Battery, EnemyType::BATTERY_MAX> m_battery;
    int m_shield;

private:
    enum {
        MOVE_POINT_MAX = 8,
        APPEARANCE_COUNT = 90,
        APPEARANCE_Z = -15,
        DESTROYED_COUNT = 90,
        DESTROYED_Z = -10,
        TIMEOUT_COUNT = 90,
        BOSS_TIMEOUT = 1800,
        ENEMY_WING_SCORE = 1000,
        SHOT_DAMAGE = 1,
        ROLL_DAMAGE = 1,
        LOCK_DAMAGE = 7
    };

    enum {
        NOHIT = -2,
        HIT = -1
    };

    static Random m_random;

    std::shared_ptr<BulletActorPool> m_bullets;
    //std::shared_ptr<ActorPool> m_shots;
    std::shared_ptr<ActorPool> m_rolls;
    std::shared_ptr<ActorPool> m_locks;
    std::shared_ptr<Ship> m_ship;
    AGameManager *m_gameManager;

    int m_cnt;

    std::shared_ptr<BulletActor> m_topBullet;
    std::shared_ptr<BulletActor> m_moveBullet;
    std::array<FVector2D, MOVE_POINT_MAX> m_movePoint;

    static const int ENEMY_TYPE_SCORE[];
    static float BOSS_MOVE_DISTANCE;

    int m_movePointNum, m_movePointIdx;
    float m_speed;
    float m_direction, m_baseDirection;
    bool m_onRoute;
    int m_fireCnt, m_barragePatternIdx;
    //float m_fieldLimitX, m_fieldLimitY;
    int m_appCnt, m_dstCnt, m_timeoutCnt;
    float m_z;
    bool m_isBoss;
    bool m_isDamaged;
    FVector2D m_velocity;
    int m_velocityCnt;
    int m_bossTimer;
};

#endif