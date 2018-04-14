#include "Field.h"

#include "GameManager.h"

using namespace std;

void Field::init() {
    m_size = FVector2D(800, 800);
    m_eyeZ = 20;
    m_roll = 0;
    m_yaw = 0;
    m_z = 10;
    m_aimZ = 10;
    m_speed = 0.1;
    m_aimSpeed = 0.1;
    m_yawYBase = 0;
    m_yawZBase = 0;
}

void Field::tick() {
    m_roll += m_speed;
    //if (m_roll >= RING_ANGLE_INT) {
    //    m_roll -= RING_ANGLE_INT;
    //}

    m_yaw += m_speed;
    m_z += (m_aimZ - m_z) * 0.003;
    m_speed += (m_aimSpeed - m_speed) * 0.004;
    m_yawYBase += (m_aimYawYBase - m_yawYBase) * 0.002;
    m_yawZBase += (m_aimYawZBase - m_yawZBase) * 0.002;
}

void Field::setType(int type) {
    switch (type) {
    case 0:
        m_aimYawYBase = 30;
        m_aimYawZBase = 0;
        break;
    case 1:
        m_aimYawYBase = 0;
        m_aimYawZBase = 20;
        break;
    case 2:
        m_aimYawYBase = 50;
        m_aimYawZBase = 10;
        break;
    case 3:
        m_aimYawYBase = 10;
        m_aimYawZBase = 30;
        break;
    }
}

bool Field::checkHit(const FVector2D &position) {
    return (position.X < -m_size.X ||
        position.X > m_size.X ||
        position.Y < -m_size.Y ||
        position.Y > m_size.Y);
}

bool Field::checkHit(const FVector2D &position, float space) {
    return (position.X < -m_size.X + space ||
        position.X > m_size.X - space ||
        position.Y < -m_size.Y + space ||
        position.Y > m_size.Y - space);
}