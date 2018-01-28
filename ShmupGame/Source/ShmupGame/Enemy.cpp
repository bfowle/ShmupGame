#include "Enemy.h"

#include "GameManager.h"
#include "BulletActor.h"
#include "BulletActorPool.h"
#include "Field.h"
#include "Lock.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"

#include "bulletml/bulletml.h"
#include "bulletml/bulletmlparser.h"

using namespace std;

const float Enemy::FIELD_SPACE = 65;
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

    m_fieldLimitX = m_field->m_size.X / 4 * 3;
    m_fieldLimitY = m_field->m_size.Y / 4 * 3;
}

void Enemy::set(const FVector2D &position, float direction, shared_ptr<EnemyType> type, BulletMLParser *moveParser) {
    m_position.X = position.X;
    m_position.Y = position.Y;
    m_type = type;

    BulletMLRunner *moveRunner = BulletMLRunner_new_parser(moveParser);
    BulletActorPool::registerFunctions(moveRunner);
    m_moveBullet = m_bullets->addBullet(moveRunner, m_position.X, m_position.Y, direction, 0, 0.5, 1, 0, 0, 1, 1);
    if (!m_moveBullet) {
        return;
    }

    m_actor = m_gameManager->m_world->SpawnActor<AActor>(m_gameManager->bp_enemyClass,
        FVector::ZeroVector, FRotator::ZeroRotator);
    //m_moveBullet->m_actor = m_actor;

    m_cnt = 0;
    m_shield = type->m_shield;

    for (int i = 0; i < type->m_batteryNum; ++i) {
        m_battery[i].m_shield = type->m_batteryType[i].m_shield;
    }

    m_fireCnt = 0;
    m_barragePatternIdx = 0;
    m_baseDirection = direction;
    m_appCnt = 0;
    m_dstCnt = 0;
    m_timeoutCnt = 0;
    m_z = 0;
    m_isBoss = false;
    m_exists = true;
}

void Enemy::setBoss(const FVector2D &p, float direction, shared_ptr<EnemyType> type) {

}

void Enemy::tick() {
    EnemyType::m_exists[m_type->m_id] = true;

    if (!m_isBoss) {
        m_position.X = m_moveBullet->m_bullet->m_position.X;
        m_position.Y = m_moveBullet->m_bullet->m_position.Y;

        //UE_LOG(LogTemp, Warning, TEXT("- Enemy::tick [%s]"), *m_position.ToString());

        if (m_actor) {
            m_actor->SetActorLocation(FVector(m_position.X, 100.0, m_position.Y));
        }
    } else {
        tickBoss();
    }

    if (m_topBullet) {
        m_topBullet->m_bullet->m_position.X = m_position.X;
        m_topBullet->m_bullet->m_position.Y = m_position.Y;
    }

    m_isDamaged = false;

    for (int i = 0; i < m_type->m_batteryNum; ++i) {
        const BatteryType *bt = &(m_type->m_batteryType[i]);
        Battery *battery = &(m_battery[i]);
        battery->m_isDamaged = false;
        for (int j = 0; j < bt->m_batteryNum; ++j) {
            if (battery->m_topBullet[j]) {
                battery->m_topBullet[j]->m_bullet->m_position.X = m_position.X + bt->m_batteryPosition[j].X;
                battery->m_topBullet[j]->m_bullet->m_position.Y = m_position.Y + bt->m_batteryPosition[j].Y;
            }
        }
    }

    if (!m_isBoss) {
        if (m_field->checkHit(m_position)) {
            //UE_LOG(LogTemp, Warning, TEXT(" __ [%s] hit field __ [%s] "), *m_actor->GetName(), *m_position.ToString());
            remove();

            if (m_actor != nullptr) {
                m_actor->Destroy();
            }
            return;
        }

        // @TODO: check that this is the correct Y [Z] positioning
        if (m_position.Y < -m_field->m_size.Y / 4) {
            removeTopBullets();
        } else {
            controlFireCnt();
        }
    } else {
        float mtr;
        if (m_appCnt > 0) {
            if (m_z < 0) {
                m_z -= APPEARANCE_Z / 60;
            }

            m_appCnt--;
            mtr = 1.0 - (float)m_appCnt / APPEARANCE_COUNT;
        } else if (m_dstCnt > 0) {
            m_gameManager->clearBullets();
            m_z += DESTROYED_Z / 60;
            m_dstCnt--;
            if (m_dstCnt <= 0) {
                remove();
                //m_gameManager->setBossShieldMeter(0, 0, 0, 0, 0, 0);
                return;
            }
            mtr = static_cast<float>(m_dstCnt) / DESTROYED_COUNT;
        } else if (m_timeoutCnt > 0) {
            m_z += DESTROYED_Z / 60;
            --m_timeoutCnt;
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
        //m_gameManager->setBossShieldMeter(shield, battery[0].shield, battery[1].shield, battery[2].shield, battery[3].shield, mtr);
    }

    ++m_cnt;

    //if (appCnt <= 0 &&
    //    dstCnt <= 0 &&
    //    timeoutCnt <= 0) {
    //    checkDamage();
    //}
}

void Enemy::tickBoss() {

}

shared_ptr<BulletActor> Enemy::setBullet(const Barrage &barrage, const FVector2D *ofs, float xr) {
    if (barrage.m_rank <= 0) {
        shared_ptr<BulletActor> null;
        return null;
    }

    BulletMLRunner *runner = BulletMLRunner_new_parser(barrage.m_parser);
    BulletActorPool::registerFunctions(runner);
    shared_ptr<BulletActor> bullet;
    float bx = m_position.X;
    float by = m_position.Y;
    if (ofs) {
        bx += ofs->X;
        by += ofs->Y;
    }
    if (barrage.m_morphCnt > 0) {
        bullet = m_bullets->addBullet(barrage.m_parser, runner,
            bx, by, m_baseDirection, 0, barrage.m_rank,
            barrage.m_speedRank, barrage.m_shape, barrage.m_color,
            barrage.m_bulletSize, barrage.m_xReverse * xr,
            barrage.m_morphParser, barrage.m_morphNum, barrage.m_morphCnt);
    } else {
        bullet = m_bullets->addBullet(barrage.m_parser, runner,
            bx, by, m_baseDirection, 0, barrage.m_rank,
            barrage.m_speedRank, barrage.m_shape, barrage.m_color,
            barrage.m_bulletSize, barrage.m_xReverse * xr);
    }

    if (bullet != nullptr) {
        bullet->m_actor = m_gameManager->m_world->SpawnActor<AActor>(m_gameManager->bp_bulletClass,
            FVector(bx, 100.0, by), FRotator::ZeroRotator);
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("- Enemy::setBullet [%s] [%f, %f] (%s)"), *m_position.ToString(), bx, by, *bullet->m_actor->GetName());

    return bullet;
}

std::shared_ptr<BulletActor> Enemy::setBullet(const Barrage &barrage, const FVector2D *ofs) {
    return setBullet(barrage, ofs, 1);
}

void Enemy::setTopBullets() {
    m_topBullet = setBullet(m_type->m_barrage[m_barragePatternIdx], 0);
    for (int i = 0; i < m_type->m_batteryNum; ++i) {
        Battery *battery = &m_battery[i];
        if (battery->m_shield <= 0) {
            continue;
        }

        const BatteryType *bt = &(m_type->m_batteryType[i]);
        float xr = 1;
        for (int j = 0; j < bt->m_batteryNum; ++j) {
            battery->m_topBullet[j] = setBullet(bt->m_barrage[m_barragePatternIdx], &(bt->m_batteryPosition[j]), xr);
            if (bt->m_xReverseAlternate)
                xr *= -1;
        }
    }
}

void Enemy::removeBattery(Battery *battery, const BatteryType &bt) {
    for (int i = 0; i < bt.m_batteryNum; i++) {
        if (battery->m_topBullet[i]) {
            battery->m_topBullet[i]->remove();
            //battery.m_topBullet[i] = 0;
            shared_ptr<BulletActor> null;
            battery->m_topBullet[i] = null;
        }
    }
    battery->m_isDamaged = true;
}

void Enemy::addDamageBattery(int idx, int dmg) {
    m_battery[idx].m_shield -= dmg;
    if (m_battery[idx].m_shield <= 0) {
        FVector2D *p = &(m_type->m_batteryType[idx].m_collisionPosition);

        removeBattery(&(m_battery[idx]), m_type->m_batteryType[idx]);
        m_velocity.Y = -p->X / 10;
        m_velocity.Y = -p->X / 10;
        m_velocityCnt = 60;

        removeTopBullets();

        m_fireCnt = m_velocityCnt + 10;
    }
}

int Enemy::checkLocked(const FVector2D &position, float xofs, shared_ptr<Lock> lock) {
    if (fabs(position.X - m_position.Y) < m_type->m_collisionSize.X + xofs &&
        m_position.Y < lock->m_lockMinY &&
        m_position.Y > position.Y) {
        return HIT;
    }

    if (m_type->m_wingCollision) {
        int lp = NOHIT;
        for (int i = 0; i < m_type->m_batteryNum; ++i) {
            if (m_battery[i].m_shield <= 0) {
                continue;
            }

            const BatteryType *bt = &(m_type->m_batteryType[i]);
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

    for (int i = 0; i < m_type->m_batteryNum; ++i) {
        const BatteryType *bt = &(m_type->m_batteryType[i]);
        Battery *battery = &(m_battery[i]);
        for (int j = 0; j < bt->m_batteryNum; j++) {
            if (battery->m_topBullet[j]) {
                battery->m_topBullet[j]->remove();
                battery->m_topBullet[j].reset();
            }
        }
    }
}

void Enemy::remove() {
    removeTopBullets();

    if (m_moveBullet) {
        m_moveBullet->remove();
    }

    m_exists = false;
}

void Enemy::gotoNextPoint() {
    m_onRoute = false;

    m_movePointIdx++;
    if (m_movePointIdx >= m_movePointNum) {
        m_movePointIdx = 0;
    }
}

void Enemy::controlFireCnt() {
    if (m_fireCnt <= 0) {
        setTopBullets();

        m_fireCnt = m_type->m_fireInterval;

        ++m_barragePatternIdx;
        if (m_barragePatternIdx >= m_type->m_barragePatternNum) {
            m_barragePatternIdx = 0;
        }
    } else if (m_fireCnt < m_type->m_fireInterval - m_type->m_firePeriod) {
        removeTopBullets();
    }
    --m_fireCnt;
}