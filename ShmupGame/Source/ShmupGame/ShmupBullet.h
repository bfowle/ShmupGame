#ifndef SHMUPBULLET_H
#define SHMUPBULLET_H

#include "MorphBullet.h"

class ShmupBullet : public MorphBullet {
public:
    explicit ShmupBullet(int id) :
        MorphBullet(id) {
    };

    enum { ORANGE, PINK };

private:
    ShmupBullet(const ShmupBullet &other);
    ShmupBullet &operator=(const ShmupBullet &other);

public:
    void setParam(float speedRank, float xReverse);

public:
    float m_speedRank;
    float m_xReverse;
    int m_color;
};

#endif