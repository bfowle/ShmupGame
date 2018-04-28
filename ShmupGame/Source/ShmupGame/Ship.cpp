#include "Ship.h"

#include "Bullet.h"
#include "Field.h"
#include "GameManager.h"

using namespace std;

bool Ship::m_isSlow = false;
const float Ship::SIZE = 0.3;
Random Ship::m_random;
const float Ship::BASE_SPEED = 0.6;
const float Ship::SLOW_BASE_SPEED = 0.3;
const float Ship::FIRE_WIDE_BASE_DIRECTION = 0.7;
const float Ship::FIRE_NARROW_BASE_DIRECTION = 0.5;
const float Ship::TURRET_INTERVAL_LENGTH = 0.2;
const float Ship::FIELD_SPACE = 1.5;

void Ship::init(shared_ptr<Field> field, TWeakObjectPtr<AGameManager> gameManager) {
    m_field = field;
    m_gameManager = gameManager;
    m_position = FVector();
    m_prevPosition = FVector();
    m_velocity = FVector();
    m_firePosition = FVector();

    m_ttlCnt = 0;
    m_fieldLimitX = m_field->m_size.X - FIELD_SPACE;
    m_fieldLimitY = m_field->m_size.Y - FIELD_SPACE;
}

void Ship::start() {
    m_position.X = 0;
    m_position.Z = -m_field->m_size.Y / 2;
    m_prevPosition = FVector(0, 0, 0);
    m_velocity = FVector(0, 0, 0);
    m_speed = BASE_SPEED;
    m_fireWideDirection = FIRE_WIDE_BASE_DIRECTION;
    m_restart = true;
    m_cnt = -INVINCIBLE_COUNT;
    m_fireCnt = 0;
    m_rollLockCnt = 0;
    m_bank = 0;
    m_rollCharged = false;
}

void Ship::setSpeedRate(float rate) {
    if (!m_isSlow) {
        m_baseSpeed = BASE_SPEED * rate;
    } else {
        m_baseSpeed = BASE_SPEED * 0.7;
    }
    m_slowSpeed = SLOW_BASE_SPEED * rate;
}

void Ship::destroyed() {
    if (m_cnt <= 0) {
        return;
    }

    m_gameManager->shipDestroyed();
    
    //for (int i = 0; i < 45; i++) {
    //    m_gameManager->addParticle(...);
    //}

    start();
    m_cnt = -RESTART_COUNT;
}

void Ship::tick() {
    m_cnt++;
    if (m_cnt < -INVINCIBLE_COUNT) {
        return;
    } else if (m_cnt == 0) {
        m_restart = false;
    }

    if (m_playerPawn.IsValid()) {
        m_position = m_playerPawn->GetActorLocation();
    }

    /*
    m_velocity = FVector(0, 0, 0);
    m_prevPosition = m_position;
    m_position += m_velocity;
    */

    //m_bank += (m_velocity.X * BANK_BASE - m_bank) * 0.1;

    /*
    if (m_position.X < -m_fieldLimitX) {
        m_position.X = -m_fieldLimitX;
    } else if (m_position.X > m_fieldLimitX) {
        m_position.X = m_fieldLimitX;
    }
    if (m_position.Y < -m_fieldLimitY) {
        m_position.Y = -m_fieldLimitY;
    } else if (m_position.Y > m_fieldLimitY) {
        m_position.Y = m_fieldLimitY;
    }
    */

    Bullet::m_target = m_position;

    m_ttlCnt++;
}
