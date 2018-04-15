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
class Ship;
class UProjectileMovementComponent;

class EnemyInitializer : public ActorInitializer {
public:
    EnemyInitializer(std::shared_ptr<Field> field, std::shared_ptr<Ship> ship, std::shared_ptr<BulletActorPool> bullets, /*std::shared_ptr<ActorPool> shots,*/ TWeakObjectPtr<AGameManager> gameManager) :
        m_field(field),
        m_ship(ship),
        m_bullets(bullets),
        //m_shots(shots),
        m_gameManager(gameManager) {
    }

public:
    std::shared_ptr<Field> m_field;
    std::shared_ptr<Ship> m_ship;
    std::shared_ptr<BulletActorPool> m_bullets;
    //std::shared_ptr<ActorPool> m_shots;
    TWeakObjectPtr<AGameManager> m_gameManager;
};

class Enemy : public Actor, public std::enable_shared_from_this<Enemy> {
public:
    std::shared_ptr<Enemy> getPtr();
    std::shared_ptr<Actor> newActor();

    void init(std::shared_ptr<ActorInitializer> initializer);
    void set(const FVector2D &position, float direction, BulletMLParser *parser);
    void setBoss(const FVector2D &p, float direction);
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
    void removeFormation(EnemyFormation *formation, const EnemyFormationType &ft);
    void remove();

public:
    static const float FIELD_SPACE;

    TWeakObjectPtr<AActor> m_actor;
    FVector2D m_position;
    std::shared_ptr<EnemyType> m_type;
    std::array<EnemyFormation, EnemyType::FORMATION_MAX> m_formation;
    int m_shield;

private:
    TWeakObjectPtr<UProjectileMovementComponent> m_movement;
    BulletMLParser *m_moveParser;

    std::shared_ptr<Field> m_field;
    std::shared_ptr<BulletActorPool> m_bullets;
    //std::shared_ptr<ActorPool> m_shots;
    std::shared_ptr<Ship> m_ship;
    TWeakObjectPtr<AGameManager> m_gameManager;

    std::shared_ptr<BulletActor> m_topBullet;
    std::shared_ptr<BulletActor> m_moveBullet;

    float m_speed;
    float m_direction, m_baseDirection;

    // enemy
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