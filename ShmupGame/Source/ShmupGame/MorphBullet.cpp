#include "MorphBullet.h"

#include <bulletml/bulletmlparser.h>

using namespace std;

void MorphBullet::setMorph(array<BulletMLParser *, MorphBullet::MORPH_MAX> mrp, int num, int idx, int cnt) {
    if (cnt <= 0) {
        m_isMorph = false;
        return;
    }
    m_isMorph = true;
    m_morphCnt = cnt;
    m_baseMorphCnt = cnt;
    m_morphNum = num;

    for (int i = 0; i < num; ++i) {
        m_morphParser[i] = mrp[i];
    }

    m_morphIdx = idx;
    if (m_morphIdx >= m_morphNum) {
        m_morphIdx = 0;
    }
    m_baseMorphCnt = m_morphIdx;
}

void MorphBullet::resetMorph() {
    m_morphIdx = m_baseMorphIdx;
    m_morphCnt = m_baseMorphCnt;
}