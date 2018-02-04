#ifndef ENEMY_H
#define ENEMY_H

#include "Actor.h"
#include "Enemy.h"
#include "EnemyType.h"
#include "Random.h"

#include "Math/Vector.h"

#include <array>
#include <memory>

class AActor;
class AGameManager;
class ActorPool;
class BulletActor;
class BulletActorPool;
class Field;
class Lock;
class Ship;
class UProjectileMovementComponent;

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

class Enemy : public Actor, public std::enable_shared_from_this<Enemy> {
public:
    std::shared_ptr<Enemy> getPtr();
    std::shared_ptr<Actor> newActor();

    void init(std::shared_ptr<ActorInitializer> initializer);
    void set(const FVector2D &position, float direction, std::shared_ptr<EnemyType> type, BulletMLParser *parser);
    void setBoss(const FVector2D &p, float direction, std::shared_ptr<EnemyType> type);
    void setActor(TWeakObjectPtr<AActor> actor);
    void tick();
    void tickBoss();

    inline bool shouldSpawnActor() { return m_moveBullet != nullptr; }

private:
    struct EnemyFormation {
        std::array<std::shared_ptr<BulletActor>, EnemyFormationType::WING_FORMATION_MAX> m_topBullet;
        int m_shield;
        bool m_isDamaged;
    };

    std::shared_ptr<BulletActor> setBullet(const Barrage &barrage, const FVector2D *offset, float xReverse);
    std::shared_ptr<BulletActor> setBullet(const Barrage &barrage, const FVector2D *offset);
    void setTopBullets();
    void removeFormation(EnemyFormation *formation, const EnemyFormationType &ft);
    //void addDamageFormation(int idx, int dmg);
    //int checkHit(const FVector2D &position, float xofs, float yofs);
    int checkLocked(const FVector2D &position, float xofs, std::shared_ptr<Lock> lock);
    void removeTopBullets();
    void remove();
    void gotoNextPoint();
    void controlFireCnt();

public:
    static const float FIELD_SPACE;

    FVector2D m_position;
    std::shared_ptr<EnemyType> m_type;
    std::array<EnemyFormation, EnemyType::FORMATION_MAX> m_formation;
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
    static const int ENEMY_TYPE_SCORE[];
    static float BOSS_MOVE_DISTANCE;

    TWeakObjectPtr<AActor> m_actor;
    TWeakObjectPtr<UProjectileMovementComponent> m_movement;

    std::shared_ptr<Field> m_field;
    std::shared_ptr<BulletActorPool> m_bullets;
    //std::shared_ptr<ActorPool> m_shots;
    std::shared_ptr<ActorPool> m_rolls;
    std::shared_ptr<ActorPool> m_locks;
    std::shared_ptr<Ship> m_ship;
    TWeakObjectPtr<AGameManager> m_gameManager;

    std::shared_ptr<BulletActor> m_topBullet;
    std::shared_ptr<BulletActor> m_moveBullet;
    std::array<FVector2D, MOVE_POINT_MAX> m_movePoint;

    float m_speed;
    float m_direction, m_baseDirection;

    int m_movePointSize, m_movePointIdx;
    int m_barragePatternIdx;
    bool m_onRoute;

    // enemy
    float m_cnt, m_fireCnt;
    bool m_isDamaged;
    FVector2D m_velocity;
    int m_velocityCnt;

    // boss
    float m_appearanceCnt, m_destroyedCnt, m_timeoutCnt;
    bool m_isBoss;
    int m_bossTimer;

    //float m_fieldLimitX, m_fieldLimitY;
};

#endif