#include "EnemyType.h"

#include "BarrageManager.h"

#include <cassert>

using namespace std;

Barrage::Barrage() :
    m_parser(nullptr) {
    for (int i = 0; i < m_morphParser.size(); ++i) {
        m_morphParser[i] = nullptr;
    }
}

//-----------------------------------------------------------------------------

array<bool, EnemyType::ENEMY_TYPE_MAX> EnemyType::m_exists;

Random EnemyType::m_random;
shared_ptr<BarrageManager> EnemyType::m_barrageManager(new BarrageManager());
array<bool, BarrageManager::BARRAGE_MAX> EnemyType::m_usedMorphParser;
int EnemyType::m_idCnt;

EnemyType::EnemyType() {
    assert(m_idCnt < ENEMY_TYPE_MAX);
    m_id = m_idCnt;
    ++m_idCnt;
}

void EnemyType::init(shared_ptr<BarrageManager> barrageManager) {
    m_random = Random();
    m_barrageManager = barrageManager;

    m_idCnt = 0;
}

void EnemyType::clearExistsList() {
    for (int i = 0; i < m_idCnt; ++i) {
        m_exists[i] = false;
    }
}

void EnemyType::setSmallEnemyType(float rank) {
    m_type = SMALL;
    m_barragePatternSize = 1;

    Barrage *barrage = &(m_barrage[0]);
    setBarrageType(barrage, BarrageManager::SMALL);
    setBarrageRank(barrage, rank, VERY_WEAK);

    barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

    setFormation(0, 0, 0, NORMAL, 0, 0, 1);

    m_shield = 1;
}

void EnemyType::setMediumEnemyType(float rank) {
    m_type = MEDIUM;
    m_barragePatternSize = 1;
    
    Barrage *barrage = &(m_barrage[0]);
    setBarrageType(barrage, BarrageManager::MEDIUM);

    float cr = 0;
    float sr = 0;
    switch (m_random.nextInt(6)) {
    case 0:
    case 1:
        cr = rank / 3 * 2;
        sr = 0;
        break;
    case 2:
        cr = rank / 4;
        sr = rank / 4;
        break;
    case 3:
    case 4:
    case 5:
    default:
        cr = 0;
        sr = rank / 2;
        break;
    }

    setBarrageRank(barrage, cr, MORPH_WEAK);

    barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

    m_shield = 40 + m_random.nextInt(10);
    setFormation(sr, 1, BarrageManager::MEDIUM_SUB, NORMAL, 0, 0, 1);
}

void EnemyType::setLargeEnemyType(float rank) {
    m_type = LARGE;

    m_barragePatternSize = 2 + m_random.nextInt(3);

    //setEnemyColorType();

    int bn1 = 1 + m_random.nextInt(3);
    int bn2 = 1 + m_random.nextInt(3);

    for (int i = 0; i < m_barragePatternSize; i++) {
        Barrage *barrage = &(m_barrage[i]);
        setBarrageType(barrage, BarrageManager::LARGE);

        float cr = 0;
        float sr1 = 0;
        float sr2 = 0;
        switch (m_random.nextInt(3)) {
        case 0:
            cr = rank;
            sr1 = 0;
            sr2 = 0;
            break;
        case 1:
            cr = rank / 3;
            sr1 = rank / 3;
            sr2 = 0;
            break;
        case 2:
        default:
            cr = rank / 3;
            sr1 = 0;
            sr2 = rank / 3;
            break;
        }

        setBarrageRankSlow(barrage, cr, NORMAL, 0.9);

        barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

        setFormation(sr1, bn1, BarrageManager::MEDIUM, NORMAL, 0, i, 0.9);
        setFormation(sr2, bn2, BarrageManager::MEDIUM, NORMAL, 2, i, 0.9);
    }

    m_shield = 400 + m_random.nextInt(50);
}

void EnemyType::setMediumBossEnemyType(float rank) {
    m_type = MEDIUM;

    m_barragePatternSize = 2 + m_random.nextInt(2);

    int bn = 1 + m_random.nextInt(2);

    for (int i = 0; i < m_barragePatternSize; ++i) {
        Barrage *barrage = &(m_barrage[i]);
        setBarrageType(barrage, BarrageManager::LARGE);

        float cr = 0;
        float sr = 0;
        switch (m_random.nextInt(3)) {
        case 0:
            cr = rank;
            sr = 0;
            break;
        case 1:
            cr = rank / 3;
            sr = rank / 3;
            break;
        case 2:
            cr = 0;
            sr = rank;
            break;
        }

        setBarrageRankSlow(barrage, cr, NORMAL, 0.9);

        barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

        setFormation(sr, bn, MEDIUM, WEAK, 0, i, 0.9);
    }

    m_shield = 300 + m_random.nextInt(50);
}

void EnemyType::setLargeBossEnemyType(float rank) {
    m_type = LARGE;

    m_barragePatternSize = 2 + m_random.nextInt(3);

    int bn1 = 1 + m_random.nextInt(3);
    int bn2 = 1 + m_random.nextInt(3);

    for (int i = 0; i < m_barragePatternSize; ++i) {
        Barrage *barrage = &(m_barrage[i]);
        setBarrageType(barrage, BarrageManager::LARGE);

        float cr = 0;
        float sr1 = 0;
        float sr2 = 0;
        switch (m_random.nextInt(3)) {
        case 0:
            cr = rank;
            sr1 = 0;
            sr2 = 0;
            break;
        case 1:
            cr = rank / 3;
            sr1 = rank / 3;
            sr2 = 0;
            break;
        case 2:
            cr = rank / 3;
            sr1 = 0;
            sr2 = rank / 3;
            break;
        }

        setBarrageRankSlow(barrage, cr, NORMAL, 0.9);

        barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

        setFormation(sr1, bn1, MEDIUM, NORMAL, 0, i, 0.9);
        setFormation(sr2, bn2, MEDIUM, NORMAL, 2, i, 0.9);
    }

    m_shield = 400 + m_random.nextInt(50);
}

void EnemyType::setBarrageType(Barrage *barrage, int btn) {
    //barrage->m_parser = reinterpret_cast<BulletMLParser *>(
    //    m_barrageManager->m_parser[btn][m_random.nextInt(m_barrageManager->m_parserSizes[btn])]);
    
    for (int i = 0; i < BarrageManager::BARRAGE_MAX; ++i) {
        m_usedMorphParser[i] = false;
    }

    /*
    int mpn;
    mpn = m_barrageManager->m_parserSizes[BarrageManager::MORPH];

    for (int i = 0; i < barrage->m_morphParser.size(); ++i) {
        int mi = m_random.nextInt(mpn);
        for (int j = 0; i < mpn; ++j) {
            if (!m_usedMorphParser[mi]) {
                break;
            }
            ++mi;

            if (mi >= mpn) {
                mi = 0;
            }
        }

        //barrage->m_morphParser[i] = reinterpret_cast<BulletMLParser *>(
        //    m_barrageManager->m_parser[BarrageManager::MORPH][mi]);

        m_usedMorphParser[mi] = true;
    }

    barrage->m_morphSize = barrage->m_morphParser.size();
    */
}

void EnemyType::setBarrageRank(Barrage *barrage, float rank, int intensity) {
    if (rank <= 0) {
        barrage->m_rank = 0;
        return;
    }

    barrage->m_rank = sqrt(rank) / (8.0 - m_random.nextInt(3));
    if (barrage->m_rank > 0.8) {
        barrage->m_rank = m_random.nextFloat(0.2) + 0.8;
    }
    rank /= (barrage->m_rank + 2.0);
    if (intensity == WEAK) {
        barrage->m_rank /= 2.0;
    }

    barrage->m_speedRank = sqrt(rank) * (m_random.nextFloat(0.2) + 1.0);
    if (barrage->m_speedRank < 1.0) {
        barrage->m_speedRank = 1.0;
    }

    if (barrage->m_speedRank > 2.0) {
        barrage->m_speedRank = sqrt(barrage->m_speedRank) + 0.27;
    }

    barrage->m_morphRank = rank / barrage->m_speedRank;
    barrage->m_morphCnt = 0;
    while (barrage->m_morphRank > 1.0) {
        barrage->m_morphCnt++;
        barrage->m_morphRank /= 3.0;
    }

    if (intensity == VERY_WEAK) {
        barrage->m_morphRank /= 2.0;
        barrage->m_morphCnt /= 1.7;
    } else if (intensity == MORPH_WEAK) {
        barrage->m_morphRank /= 2.0;
    } else if (intensity == WEAK) {
        barrage->m_morphRank /= 1.5;
    }
}

void EnemyType::setBarrageRankSlow(Barrage *barrage, float rank, int intensity, float slowness) {
    setBarrageRank(barrage, rank, intensity);
    barrage->m_speedRank *= slowness;
}

void EnemyType::setFormation(float rank, int n, int barrageType, int barrageIntensity, int idx, int ptnIdx, float slowness) {
    EnemyFormationType *bt = &(m_formationType[idx]);
    EnemyFormationType *bt2 = &(m_formationType[idx + 1]);
    Barrage *barrage = &(bt->m_barrage[ptnIdx]);
    Barrage *barrage2 = &(bt2->m_barrage[ptnIdx]);

    setBarrageType(barrage, barrageType);
    setBarrageRankSlow(barrage, rank / n, barrageIntensity, slowness);
    //setBarrageShape(barrage, 0.8);

    barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

    barrage2->m_parser = barrage->m_parser;
    for (int i = 0; i < MorphBullet::MORPH_MAX; ++i) {
        barrage2->m_morphParser[i] = barrage->m_morphParser[i];
    }

    barrage2->m_morphSize = barrage->m_morphSize;
    barrage2->m_morphCnt = barrage->m_morphCnt;
    barrage2->m_rank = barrage->m_rank;
    barrage2->m_speedRank = barrage->m_speedRank;
    barrage2->m_morphRank = barrage->m_morphRank;
    //barrage2->m_bulletSize = barrage->m_bulletSize;
    barrage2->m_xReverse = -barrage->m_xReverse;
    
    if (m_random.nextInt(4) == 0) {
        bt->m_xReverseAlternate = true;
        bt2->m_xReverseAlternate = true;
    } else {
        bt->m_xReverseAlternate = false;
        bt2->m_xReverseAlternate = false;
    }

    float px = bt->m_wingShapePosition[1].X;
    float py = bt->m_wingShapePosition[1].Y;
    float mpx = bt->m_wingShapePosition[2].X;
    float mpy = bt->m_wingShapePosition[2].Y;
    for (int i = 0; i < n; i++) {
        bt->m_formationPosition[i].X = px;
        bt->m_formationPosition[i].Y = py;
        bt2->m_formationPosition[i].X = -px;
        bt2->m_formationPosition[i].Y = py;
        px += (mpx - px) / (n - 1);
        py += (mpy - py) / (n - 1);
    }

    bt->m_formationSize = n;
    bt2->m_formationSize = n;
}