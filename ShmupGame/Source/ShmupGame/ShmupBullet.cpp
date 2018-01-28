#include "ShmupBullet.h"

void ShmupBullet::setParam(float speedRank, int shape, int color, float bulletSize, float xReverse) {
    m_speedRank = speedRank;
    m_shape = shape;
    m_color = color;
    m_bulletSize = bulletSize;
    m_xReverse = xReverse;
}