#ifndef SHMUPBULLET_H
#define SHMUPBULLET_H

#include "MorphBullet.h"

class ShmupBullet : public MorphBullet {
public:
    explicit ShmupBullet(int id) : MorphBullet(id) {};

private:
    ShmupBullet(const ShmupBullet &other);
    ShmupBullet &operator=(const ShmupBullet &other);

public:
    void setParam(float speedRank, int shape, int color, float bulletSize, float xReverse);

public:
    float m_speedRank;
    int m_shape, m_color;
    float m_bulletSize;
    float m_xReverse;
};

#endif