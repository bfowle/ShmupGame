#include "MorphBullet.h"

#include <bulletml/bulletmlparser.h>

using namespace std;

void MorphBullet::setMorph(array<BulletMLParser *, MorphBullet::MORPH_MAX> parser, int size, int idx, int cnt) {
    if (cnt <= 0) {
        m_isMorph = false;
        return;
    }
    m_isMorph = true;
    m_morphCnt = cnt;
    m_baseMorphCnt = cnt;
    m_morphSize = size;

    for (int i = 0; i < size; ++i) {
        m_morphParser[i] = parser[i];
    }

    m_morphIdx = idx;
    if (m_morphIdx >= m_morphSize) {
        m_morphIdx = 0;
    }
    m_baseMorphCnt = m_morphIdx;
}

void MorphBullet::resetMorph() {
    m_morphIdx = m_baseMorphIdx;
    m_morphCnt = m_baseMorphCnt;
}