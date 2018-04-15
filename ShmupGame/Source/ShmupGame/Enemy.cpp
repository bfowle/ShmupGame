#include "Enemy.h"

#include "GameManager.h"
#include "BulletActor.h"
#include "BulletActorPool.h"
#include "Field.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "bulletml/bulletml.h"
#include "bulletml/bulletmlparser.h"

using namespace std;

const float Enemy::FIELD_SPACE = 90.0;

shared_ptr<Enemy> Enemy::getPtr() {
    return shared_from_this();
}

shared_ptr<Actor> Enemy::newActor() {
    return shared_ptr<Actor>(new Enemy());
}

void Enemy::init(shared_ptr<ActorInitializer> initializer) {
    shared_ptr<EnemyInitializer> enemy = static_pointer_cast<EnemyInitializer>(initializer);
    m_field = enemy->m_field;
    m_ship = enemy->m_ship;
    m_bullets = enemy->m_bullets;
    //m_shots = enemy->m_shots;
    m_gameManager = enemy->m_gameManager;

    m_position = FVector2D();

    m_velocity = FVector2D();
    m_velocityCnt = 0;

    //m_fieldLimitX = m_field->m_size.X / 4 * 3;
    //m_fieldLimitY = m_field->m_size.Y / 4 * 3;
}

void Enemy::set(const FVector2D &position, float direction/*, shared_ptr<EnemyType> type*/, BulletMLParser *moveParser) {
    m_position = position;

    m_moveBullet = m_bullets->addBullet(moveParser, m_position.X, m_position.Y, direction, 0, 0.5, 1, 1);
    if (!m_moveBullet) {
        return;
    }

    /*
    m_shield = type->m_shield;
    for (int i = 0; i < type->m_formationSize; ++i) {
        m_formation[i].m_shield = type->m_formationType[i].m_shield;
    }
    */

    m_baseDirection = direction;
    m_appearanceCnt = 0;
    m_destroyedCnt = 0;
    m_timeoutCnt = 0;
    m_isBoss = false;
    m_isAlive = true;
}

void Enemy::setBoss(const FVector2D &p, float direction/*, shared_ptr<EnemyType> type*/) {
}

void Enemy::setActor(TWeakObjectPtr<AActor> actor) {
    m_actor = actor;
    if (m_moveBullet) {
        m_moveBullet->setActor(actor);
    }

    if (m_actor.IsValid()) {
        m_uuid = actor->GetUniqueID();
        m_movement = m_actor->FindComponentByClass<UProjectileMovementComponent>();
    }
}

void Enemy::tick() {
    //EnemyType::m_exists[m_type->m_id] = true;

    if (!m_isBoss) {
        //m_position = m_moveBullet->m_bullet->m_position;

        if (m_actor.IsValid() && 
            m_movement.IsValid() &&
            m_movement->UpdatedComponent) {
            FVector vel = (m_movement->UpdatedComponent->GetForwardVector() *
                (sin(m_moveBullet->m_bullet->m_direction) +
                    m_moveBullet->m_bullet->m_acceleration.X) *
                    m_moveBullet->m_bullet->m_xReverse) +
                (m_movement->UpdatedComponent->GetUpVector() *
                (cos(m_moveBullet->m_bullet->m_direction) +
                    m_moveBullet->m_bullet->m_acceleration.Y));
            vel.Y = 0;

            if (!vel.IsNearlyZero()) {
                vel.Normalize();
                vel *= m_moveBullet->m_bullet->m_speed *
                    m_movement->GetMaxSpeed() *
                    m_gameManager->m_deltaSeconds;
                m_movement->MoveUpdatedComponent(vel, FRotator::ZeroRotator, true);
                if (m_movement.IsValid()) {
                    m_movement->UpdateComponentVelocity();
                }
            }

            if (m_actor.IsValid()) {
                m_position.X = m_actor->GetActorLocation().X;
                m_position.Y = m_actor->GetActorLocation().Z;
            }
        }
    } else {
        tickBoss();
    }
    
    if (m_moveBullet) {
        m_moveBullet->m_bullet->m_position = m_position;
    }
    if (m_topBullet) {
        m_topBullet->m_bullet->m_position = m_position;
    }

#if 0
    m_isDamaged = false;

    for (int i = 0; i < m_type->m_formationSize; ++i) {
        const EnemyFormationType *ft = &(m_type->m_formationType[i]);
        EnemyFormation *formation = &(m_formation[i]);
        formation->m_isDamaged = false;

        for (int j = 0; j < ft->m_formationSize; ++j) {
            if (formation->m_topBullet[j]) {
                formation->m_topBullet[j]->m_bullet->m_position.X = m_position.X + ft->m_formationPosition[j].X;
                formation->m_topBullet[j]->m_bullet->m_position.Y = m_position.Y + ft->m_formationPosition[j].Y;
            }
        }
    }

    if (!m_isBoss) {
        /*
        if (m_field->checkHit(m_position)) {
            //UE_LOG(LogTemp, Warning, TEXT(" __ [%s] hit field __ [%s] "), *m_actor->GetName(), *m_position.ToString());
            remove();

            if (m_actor.IsValid()) {
                m_gameManager->m_world->DestroyActor(m_actor.Get());
                //m_actor->Destroy();
                //m_actor->ConditionalBeginDestroy();
            }
            return;
        }
        */

        if (m_position.Y < -m_field->m_size.Y / 4) {
            //removeTopBullets();
        } else {
            controlFireCnt();
        }
    }
#endif
}

void Enemy::tickBoss() {
}

shared_ptr<BulletActor> Enemy::setBullet(const Barrage &barrage, const FVector2D *offset, float xReverse) {
    if (barrage.m_rank <= 0) {
        shared_ptr<BulletActor> null;
        return null;
    }

    shared_ptr<BulletActor> bullet;
    float bx = m_position.X;
    float by = m_position.Y;
    if (offset) {
        bx += offset->X;
        by += offset->Y;
    }

    if (barrage.m_morphCnt > 0) {
        bullet = m_bullets->addBullet(barrage.m_parser,
            bx, by, m_baseDirection, 0, barrage.m_rank,
            barrage.m_speedRank, barrage.m_xReverse * xReverse,
            barrage.m_morphParser, barrage.m_morphSize, barrage.m_morphCnt);
    } else {
        bullet = m_bullets->addBullet(barrage.m_parser,
            bx, by, m_baseDirection, 0, barrage.m_rank,
            barrage.m_speedRank, barrage.m_xReverse * xReverse);
    }

    return bullet;
}

shared_ptr<BulletActor> Enemy::setBullet(const Barrage &barrage, const FVector2D *offset) {
    return setBullet(barrage, offset, 1);
}

void Enemy::removeFormation(EnemyFormation *formation, const EnemyFormationType &ft) {
    for (int i = 0; i < ft.m_formationSize; i++) {
        if (formation->m_topBullet[i]) {
            formation->m_topBullet[i]->remove();
            shared_ptr<BulletActor> null;
            formation->m_topBullet[i] = null;
        }
    }
    formation->m_isDamaged = true;
}

void Enemy::remove() {
    if (m_moveBullet) {
        m_moveBullet->remove();
    }
    m_isAlive = false;
}