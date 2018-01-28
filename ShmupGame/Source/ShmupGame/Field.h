#ifndef FIELD_H
#define FIELD_H

#include "Math/Vector.h"

#include <array>

class AGameManager;

class Field {
public:
    void init();
    void tick();
    void setType(int type);
    //void setColor(int mode);
    bool checkHit(const FVector2D &position);
    bool checkHit(const FVector2D &position, float space);

private:
    //static void writeOneRing();

public:
    enum {
        TYPE_NUMBER = 4
    };

    FVector2D m_size;
    float m_eyeZ;
    float m_aimZ, m_aimSpeed;

private:
    /*
    enum {
        RING_NUMBER = 16,
        RING_ANGLE_INT = 10,
        RING_POSITION_NUMBER = 16,
        RING_RADIUS = 10
    };
    */

    float m_roll, m_yaw;
    float m_z;
    float m_speed;
    float m_yawYBase, m_yawZBase;
    float m_aimYawYBase, m_aimYawZBase;
    //float m_r, m_g, m_b;
};

#endif