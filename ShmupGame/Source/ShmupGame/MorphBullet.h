#ifndef MORPHBULLET_H
#define MORPHBULLET_H

#include "Bullet.h"

#include <array>
#include <vector>

class BulletMLParser;

class MorphBullet : public Bullet {
public:
    explicit MorphBullet(int id) : Bullet(id) {};

    enum {
        MORPH_MAX = 8
    };

    void setMorph(std::array<BulletMLParser *, MORPH_MAX> mrp, int num, int idx, int cnt);
    void resetMorph();

private:
    MorphBullet(const MorphBullet &other);
    MorphBullet &operator=(const MorphBullet &other);

public:
    std::array<BulletMLParser *, MORPH_MAX> m_morphParser;
    int m_morphNum;
    int m_morphIdx;
    int m_morphCnt;
    int m_baseMorphIdx;
    int m_baseMorphCnt;
    bool m_isMorph;
};

#endif