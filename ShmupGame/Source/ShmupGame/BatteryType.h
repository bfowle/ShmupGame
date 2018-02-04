#ifndef BATTERYTYPE_H
#define BATTERYTYPE_H

#include "Barrage.h"

#include "Math/Vector.h"

#include <array>
#include <memory>

class BatteryType {
public:
    enum {
        WING_SHAPE_POINT_NUM = 3,
        WING_BATTERY_MAX = 3,
        BARRAGE_PATTERN_MAX = 8
    };

    std::array<Barrage, BARRAGE_PATTERN_MAX> m_barrage;
    std::array<FVector2D, WING_BATTERY_MAX> m_batteryPosition;
    std::array<FVector2D, WING_SHAPE_POINT_NUM> m_wingShapePosition;
    int m_batterySize;
    int m_shield;

    FVector2D m_collisionPosition;
    FVector2D m_collisionSize;
    //float r, g, b;
    bool m_xReverseAlternate;
};

#endif