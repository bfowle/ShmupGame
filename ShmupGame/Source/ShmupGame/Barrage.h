#ifndef BARRAGE_H
#define BARRAGE_H

#include "MorphBullet.h"

#include <array>

class BulletMLParser;

class Barrage {
public:
    Barrage();

private:
    Barrage(const Barrage &other);
    Barrage &operator=(const Barrage &other);

public:
    BulletMLParser *m_parser;
    std::array<BulletMLParser *, MorphBullet::MORPH_MAX> m_morphParser;

    int m_morphNum, m_morphCnt;
    float m_rank, m_speedRank, m_morphRank;
    int m_shape, m_color;
    float m_bulletSize;
    float m_xReverse;
};

#endif