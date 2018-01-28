#include "Lock.h"

#include "LockInitializer.h"
#include "GameManager.h"
#include "Enemy.h"
#include "EnemyType.h"
#include "Field.h"
#include "Ship.h"

using namespace std;

Random Lock::m_random;
const float Lock::SPEED = 0.75;

shared_ptr<Actor> Lock::newActor() {
    return shared_ptr<Actor>(new Lock());
}

void Lock::init() {
    m_random = Random();
}

void Lock::init(shared_ptr<ActorInitializer> initializer) {
    shared_ptr<LockInitializer> li = static_pointer_cast<LockInitializer>(initializer);
    m_field = li->m_field;
    m_ship = li->m_ship;
    m_gameManager = li->m_gameManager;
}

void Lock::set() {
    reset();
    m_state = HIT;
    m_cnt = 0;
}

void Lock::reset() {
    for (int i = 0; i < m_position.size(); ++i) {
        m_position[i].X = m_ship->m_position.X;
        m_position[i].Y = m_ship->m_position.Y;
    }

    m_velocity.X = m_random.nextSignedFloat(1.5);
    m_velocity.Y = -2;
    m_cnt = 0;
}

void Lock::tick() {
    if (m_state == SEARCH) {
        m_exists = false;
        return;
    } else if (m_state == SEARCHED) {
        m_state = LOCKING;
    }

    if (m_state != HIT &&
        m_state != CANCELED) {
        if (m_lockedPart < 0) {
            //m_lockedPosition.X = m_lockedEnemy->m_position.X;
            //m_lockedPosition.Y = m_lockedEnemy->m_position.Y;
        } else {
            //m_lockedPosition.X = m_lockedEnemy->m_position.X + m_lockedEnemy->m_type->m_batteryType[m_lockedPart].m_collisionPosition.X;
            //m_lockedPosition.Y = m_lockedEnemy->m_position.Y + m_lockedEnemy->m_type->m_batteryType[m_lockedPart].m_collisionPosition.Y;
        }
    }

    switch (m_state) {
    case LOCKING:
        if (m_cnt >= LOCK_CNT) {
            m_state = LOCKED;
            m_cnt = 0;
        }
        break;

    case LOCKED:
        if (m_cnt >= NO_COLLISION_COUNT) {
            m_state = FIRED;
        }
        //break;

    case FIRED:
    case CANCELED:
        if (m_state != CANCELED) {
            if (!m_lockedEnemy->m_exists ||
                m_lockedEnemy->m_shield <= 0 ||
                (m_lockedPart >= 0 &&
                 m_lockedEnemy->m_battery[m_lockedPart].m_shield <= 0)) {
                m_state = CANCELED;
            } else {
                m_velocity.X += (m_lockedPosition.X - m_position[0].X) * SPEED;
                m_velocity.Y += (m_lockedPosition.Y - m_position[0].Y) * SPEED;
            }
            m_velocity.X *= 0.9;
            m_velocity.Y *= 0.9;

            m_position[0].X += (m_lockedPosition.X - m_position[0].X) * 0.002 * m_cnt;
            m_position[0].Y += (m_lockedPosition.Y - m_position[0].Y) * 0.002 * m_cnt;
        } else {
            m_velocity.Y += (m_field->m_size.Y * 2 - m_position[0].Y) * SPEED;
        }

        for (int i = LENGTH - 1; i > 0; --i) {
            m_position[i].X = m_position[i - 1].X;
            m_position[i].Y = m_position[i - 1].Y;
        }
        m_position[0].X += m_velocity.X;
        m_position[0].Y += m_velocity.Y;

        if (m_position[0].Y > m_field->m_size.Y + 5) {
            if (m_state == CANCELED) {
                m_exists = false;
                return;
            } else {
                m_state = LOCKED;
                reset();
            }
        }

        //float d = atan2(m_position[1].Y - m_position[0].Y,
        //    m_position[1].Y - m_position[0].Y);
        //m_gameManager->addParticle(...);
        break;

    case HIT:
        for (int i = 1; i < LENGTH; ++i) {
            m_position[i].X = m_position[i - 1].X;
            m_position[i].Y = m_position[i - 1].Y;
        }

        if (m_cnt > 5) {
            if (!m_released) {
                m_state = LOCKED;
                reset();
            } else {
                m_exists = false;
                return;
            }
        }
        break;
    }
    //UE_LOG(LogTemp, Warning, TEXT(" __ lock: %f, %f ; %f, %f __ "), m_position[0].X, m_position[0].Y, m_velocity[0].X, m_velocity[0].Y);

    m_cnt++;
}