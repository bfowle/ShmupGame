#include "Enemy.h"

#include "GameManager.h"
#include "BulletActor.h"
#include "BulletActorPool.h"
#include "Field.h"
#include "Lock.h"

#include "GameFramework/ProjectileMovementComponent.h"

#include "bulletml/bulletml.h"
#include "bulletml/bulletmlparser.h"

using namespace std;

const float Enemy::FIELD_SPACE = 90.0; // 45.0; // 1.5;
Random Enemy::m_random;

const int ENEMY_TYPE_SCORE[] = {
    100,
    500,
    1000,
    5000,
    10000
};
float Enemy::BOSS_MOVE_DISTANCE = 0.02;

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
    m_rolls = enemy->m_rolls;
    m_locks = enemy->m_locks;
    m_gameManager = enemy->m_gameManager;

    m_position = FVector2D();

    for (int i = 0; i < MOVE_POINT_MAX; ++i) {
        m_movePoint[i] = FVector2D();
    }
    m_velocity = FVector2D();
    m_velocityCnt = 0;

    //m_fieldLimitX = m_field->m_size.X / 4 * 3;
    //m_fieldLimitY = m_field->m_size.Y / 4 * 3;
}

void Enemy::set(const FVector2D &position, float direction, shared_ptr<EnemyType> type, BulletMLParser *moveParser) {
    m_position = position;
    m_type = type;

    BulletMLRunner *moveRunner = BulletMLRunner_new_parser(moveParser);
    BulletActorPool::registerFunctions(moveRunner);
    m_moveBullet = m_bullets->addBullet(moveRunner, m_position.X, m_position.Y, direction, 0, 0.5, 1, 1);
    if (!m_moveBullet) {
        return;
    }

    m_cnt = 0;
    m_shield = type->m_shield;

    for (int i = 0; i < type->m_formationSize; ++i) {
        m_formation[i].m_shield = type->m_formationType[i].m_shield;
    }

    m_fireCnt = 0;
    m_barragePatternIdx = 0;
    m_baseDirection = direction;
    m_appearanceCnt = 0;
    m_destroyedCnt = 0;
    m_timeoutCnt = 0;
    m_isBoss = false;
    m_isAlive = true;
}

void Enemy::setBoss(const FVector2D &p, float direction, shared_ptr<EnemyType> type) {
}

void Enemy::setActor(TWeakObjectPtr<AActor> actor) {
    m_actor = actor;

    if (m_actor.IsValid()) {
        m_uuid = actor->GetUniqueID();
        m_movement = m_actor->FindComponentByClass<UProjectileMovementComponent>();
    }
}

void Enemy::tick() {
    EnemyType::m_exists[m_type->m_id] = true;

    if (!m_isBoss) {
        //m_position.X = m_moveBullet->m_bullet->m_position.X;
        //m_position.Y = m_moveBullet->m_bullet->m_position.Y;

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

    if (m_topBullet) {
        m_topBullet->m_bullet->m_position.X = m_position.X;
        m_topBullet->m_bullet->m_position.Y = m_position.Y;
    }

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

        // @TODO: check that this is the correct Y [Z] positioning
        if (m_position.Y < -m_field->m_size.Y / 4) {
            //removeTopBullets();
        } else {
            controlFireCnt();
        }
    } else {
        float mtr;
        if (m_appearanceCnt > 0) {
            m_appearanceCnt--;
            //m_appearanceCnt -= m_gameManager->m_deltaSeconds;
            mtr = 1.0 - (float)m_appearanceCnt / APPEARANCE_COUNT;
        } else if (m_destroyedCnt > 0) {
            m_gameManager->clearBullets();
            m_destroyedCnt--;
            //m_dstCnt -= m_gameManager->m_deltaSeconds;
            if (m_destroyedCnt <= 0) {
                remove();
                //m_gameManager->setBossShieldMeter(0, 0, 0, 0, 0, 0);
                return;
            }
            mtr = static_cast<float>(m_destroyedCnt) / DESTROYED_COUNT;
        } else if (m_timeoutCnt > 0) {
            --m_timeoutCnt;
            //m_timeoutCnt -= m_gameManager->m_deltaSeconds;
            if (m_timeoutCnt <= 0) {
                remove();
                return;
            }
            mtr = 0;
        } else {
            controlFireCnt();
            mtr = 1;
            //++m_bossTimer;
            //if (m_bossTimer > BOSS_TIMEOUT) {
            //    m_timeoutCnt = TIMEOUT_COUNT;
            //    m_shield = 0;
            //    removeTopBullets();
            //}
        }
        //m_gameManager->setBossShieldMeter(shield, formation[0].shield, formation[1].shield, formation[2].shield, formation[3].shield, mtr);
    }

    ++m_cnt;
    //m_cnt += m_gameManager->m_deltaSeconds;

    //if (appCnt <= 0 &&
    //    dstCnt <= 0 &&
    //    timeoutCnt <= 0) {
    //    checkDamage();
    //}
}

void Enemy::tickBoss() {

}

shared_ptr<BulletActor> Enemy::setBullet(const Barrage &barrage, const FVector2D *offset, float xReverse) {
    if (barrage.m_rank <= 0) {
        shared_ptr<BulletActor> null;
        return null;
    }

    BulletMLRunner *runner = BulletMLRunner_new_parser(barrage.m_parser);
    BulletActorPool::registerFunctions(runner);
    shared_ptr<BulletActor> bullet;
    float bx = m_position.X;
    float by = m_position.Y;
    if (offset) {
        bx += offset->X;
        by += offset->Y;
    }

    if (barrage.m_morphCnt > 0) {
        bullet = m_bullets->addBullet(barrage.m_parser, runner,
            bx, by, m_baseDirection, 0, barrage.m_rank,
            barrage.m_speedRank, barrage.m_xReverse * xReverse,
            barrage.m_morphParser, barrage.m_morphSize, barrage.m_morphCnt);
    } else {
        bullet = m_bullets->addBullet(barrage.m_parser, runner,
            bx, by, m_baseDirection, 0, barrage.m_rank,
            barrage.m_speedRank, barrage.m_xReverse * xReverse);
    }

    return bullet;
}

std::shared_ptr<BulletActor> Enemy::setBullet(const Barrage &barrage, const FVector2D *offset) {
    return setBullet(barrage, offset, 1);
}

// refers to the root tree bullet in bulletml defined by <action label="top" />
void Enemy::setTopBullets() {
    m_topBullet = setBullet(m_type->m_barrage[m_barragePatternIdx], 0);
    for (int i = 0; i < m_type->m_formationSize; ++i) {
        EnemyFormation *formation = &m_formation[i];
        if (formation->m_shield <= 0) {
            continue;
        }

        const EnemyFormationType *ft = &(m_type->m_formationType[i]);
        float xr = 1;
        for (int j = 0; j < ft->m_formationSize; ++j) {
            formation->m_topBullet[j] = setBullet(ft->m_barrage[m_barragePatternIdx], &(ft->m_formationPosition[j]), xr);
            if (ft->m_xReverseAlternate) {
                xr *= -1;
            }
        }
    }
}

void Enemy::removeFormation(EnemyFormation *formation, const EnemyFormationType &ft) {
    for (int i = 0; i < ft.m_formationSize; i++) {
        if (formation->m_topBullet[i]) {
            formation->m_topBullet[i]->remove();
            //formation.m_topBullet[i] = 0;
            shared_ptr<BulletActor> null;
            formation->m_topBullet[i] = null;
        }
    }
    formation->m_isDamaged = true;
}

/**
 * @TODO: not in use
 *
void Enemy::addDamageFormation(int idx, int dmg) {
    m_formation[idx].m_shield -= dmg;
    if (m_formation[idx].m_shield <= 0) {
        FVector2D *p = &(m_type->m_formationType[idx].m_collisionPosition);

        removeFormation(&(m_formation[idx]), m_type->m_formationType[idx]);
        m_velocity.Y = -p->X / 10;
        m_velocity.Y = -p->X / 10;
        m_velocityCnt = 60;

        removeTopBullets();

        m_fireCnt = m_velocityCnt + 10;
    }
}
*/

int Enemy::checkLocked(const FVector2D &position, float xofs, shared_ptr<Lock> lock) {
    if (fabs(position.X - m_position.Y) < m_type->m_collisionSize.X + xofs &&
        m_position.Y < lock->m_lockMinY &&
        m_position.Y > position.Y) {
        return HIT;
    }

    if (m_type->m_wingCollision) {
        int lp = NOHIT;
        for (int i = 0; i < m_type->m_formationSize; ++i) {
            if (m_formation[i].m_shield <= 0) {
                continue;
            }

            const EnemyFormationType *bt = &(m_type->m_formationType[i]);
            float by = m_position.Y + bt->m_collisionPosition.Y;

            if (fabs(position.X - m_position.X - bt->m_collisionPosition.X) < bt->m_collisionSize.X + xofs &&
                by < lock->m_lockMinY &&
                by > position.Y) {
                lock->m_lockMinY = by;
                lp = i;
            }
        }
        if (lp != NOHIT) {
            return lp;
        }
    }

    return NOHIT;
}

void Enemy::removeTopBullets() {
    if (m_topBullet) {
        m_topBullet->remove();
        m_topBullet.reset();
    }

    for (int i = 0; i < m_type->m_formationSize; ++i) {
        const EnemyFormationType *bt = &(m_type->m_formationType[i]);
        EnemyFormation *formation = &(m_formation[i]);
        for (int j = 0; j < bt->m_formationSize; j++) {
            if (formation->m_topBullet[j]) {
                formation->m_topBullet[j]->remove();
                formation->m_topBullet[j].reset();
            }
        }
    }
}

void Enemy::remove() {
    removeTopBullets();

    if (m_moveBullet) {
        m_moveBullet->remove();
    }
    m_isAlive = false;
}

void Enemy::gotoNextPoint() {
    m_onRoute = false;

    m_movePointIdx++;
    if (m_movePointIdx >= m_movePointSize) {
        m_movePointIdx = 0;
    }
}

void Enemy::controlFireCnt() {
    if (m_fireCnt <= 0) {
        setTopBullets();

        //UE_LOG(LogTemp, Warning, TEXT(" 1.1 - fireCnt = fireInterval ... %f, %d "), m_fireCnt, m_type->m_fireInterval);
        m_fireCnt = m_type->m_fireInterval;

        ++m_barragePatternIdx;
        if (m_barragePatternIdx >= m_type->m_barragePatternSize) {
            m_barragePatternIdx = 0;
        }
    } else if (m_fireCnt < m_type->m_fireInterval - m_type->m_firePeriod) {
        //UE_LOG(LogTemp, Warning, TEXT(" 1.2 - fireCnt => removeTopBullets [%f, %d] "), m_fireCnt, (m_type->m_fireInterval - m_type->m_firePeriod));

        removeTopBullets();
    }
    --m_fireCnt;
    //m_fireCnt -= 10.0 * m_gameManager->m_deltaSeconds;

    //UE_LOG(LogTemp, Warning, TEXT(" [%d] fire cnt: %f ... [int: %d, prd: %d]"), m_uuid, m_fireCnt,
    //    m_type->m_fireInterval, m_type->m_firePeriod);
}