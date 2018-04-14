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

array<bool, EnemyType::ENEMY_TYPE_MAX> EnemyType::m_exists;

Random EnemyType::m_random;
shared_ptr<BarrageManager> EnemyType::m_barrageManager(new BarrageManager());
array<bool, BarrageManager::BARRAGE_MAX> EnemyType::m_usedMorphParser;
int EnemyType::m_idCnt;

const float EnemyType::m_enemySize[][9] = {
    {0.3, 0.3, 0.3, 0.1, 0.1,  1.0, 0.4, 0.6, 0.9},
    {0.4, 0.2, 0.4, 0.1, 0.15, 2.2, 0.2, 1.6, 1.0},
    {0.6, 0.3, 0.5, 0.1, 0.2,  3.0, 0.3, 1.4, 1.2},
    {0.9, 0.3, 0.7, 0.2, 0.25, 5.0, 0.6, 3.0, 1.5},
    {1.2, 0.2, 0.9, 0.1, 0.3,  7.0, 0.8, 4.5, 1.5},
};

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

void EnemyType::setSmallEnemyType(float rank, int mode) {
    m_type = SMALL;
    m_barragePatternSize = 1;
    m_wingCollision = false;

    //setEnemyColorType();

    Barrage *barrage = &(m_barrage[0]);
    setBarrageType(barrage, BarrageManager::SMALL, mode);
    //setBarrageType(barrage, BarrageManager::SMALL_LOCK, mode);
    setBarrageRank(barrage, rank, VERY_WEAK, mode);
    //setBarrageShape(barrage, 0.7);

    barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

    setEnemyShapeAndWings(SMALL);
    setFormation(0, 0, 0, NORMAL, 0, 0, 1, mode);

    m_shield = 1;
    m_fireInterval = 99999;
    m_firePeriod = 150 + m_random.nextInt(40);
    if (rank < 10) {
        m_firePeriod /= (2 - rank * 0.1);
    }
}

void EnemyType::setMediumEnemyType(float rank, int mode) {
    m_type = MEDIUM;
    m_barragePatternSize = 1;
    m_wingCollision = false;
    
    //setEnemyColorType();

    Barrage *barrage = &(m_barrage[0]);
    setBarrageType(barrage, BarrageManager::MEDIUM, mode);

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
    /*
    switch (m_random.nextInt(6)) {
    case 0:
    case 1:
        cr = rank / 5;
        sr = rank / 4;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    default:
        cr = 0;
        sr = rank / 2;
        break;
    }
    */

    setBarrageRank(barrage, cr, MORPH_WEAK, mode);
    //setBarrageShape(barrage, 0.75);

    barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

    setEnemyShapeAndWings(MEDIUM);

    m_shield = 40 + m_random.nextInt(10);
    setFormation(sr, 1, BarrageManager::MEDIUM_SUB, NORMAL, 0, 0, 1, mode);
    m_fireInterval = 72 + m_random.nextInt(60);
    m_firePeriod = static_cast<int>(m_fireInterval / (1.8 + m_random.nextFloat(0.7)));
    /*
    m_shield = 30 + m_random.nextInt(8);
    m_fireInterval = 72 + m_random.nextInt(30);
    m_firePeriod = static_cast<int>(m_fireInterval / 1.2 + m_random.nextFloat(0.2));
    */

    if (rank < 10) {
        m_firePeriod /= (2 - rank * 0.1);
    }
}

void EnemyType::setLargeEnemyType(float rank, int mode) {
    m_type = LARGE;

    m_barragePatternSize = 2 + m_random.nextInt(3);
    m_wingCollision = true;

    //setEnemyColorType();

    int bn1 = 1 + m_random.nextInt(3);
    int bn2 = 1 + m_random.nextInt(3);

    for (int i = 0; i < m_barragePatternSize; i++) {
        Barrage *barrage = &(m_barrage[i]);
        setBarrageType(barrage, BarrageManager::LARGE, mode);

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

        setBarrageRankSlow(barrage, cr, NORMAL, mode, 0.9);
        //setBarrageShape(barrage, 1.0);

        barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

        setEnemyShapeAndWings(LARGE_BOSS);
        setFormation(sr1, bn1, BarrageManager::MEDIUM, NORMAL, 0, i, 0.9, mode);
        setFormation(sr2, bn2, BarrageManager::MEDIUM, NORMAL, 2, i, 0.9, mode);
    }

    m_shield = 400 + m_random.nextInt(50);
    m_fireInterval = 220 + m_random.nextInt(60);
    m_firePeriod = static_cast<int>(m_fireInterval / (1.2 + m_random.nextFloat(0.3)));
    if (rank < 10) {
        m_firePeriod /= (2 - rank * 0.1);
    }
}

void EnemyType::setMediumBossEnemyType(float rank, int mode) {
    m_type = MEDIUM;

    m_barragePatternSize = 2 + m_random.nextInt(2);
    m_wingCollision = true;

    //setEnemyColorType();

    int bn = 1 + m_random.nextInt(2);

    for (int i = 0; i < m_barragePatternSize; ++i) {
        Barrage *barrage = &(m_barrage[i]);
        setBarrageType(barrage, BarrageManager::LARGE, mode);

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

        setBarrageRankSlow(barrage, cr, NORMAL, mode, 0.9);
        //setBarrageShape(barrage, 0.9);

        barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

        setEnemyShapeAndWings(MEDIUM_BOSS);
        setFormation(sr, bn, MEDIUM, WEAK, 0, i, 0.9, mode);
    }

    m_shield = 300 + m_random.nextInt(50);
    m_fireInterval = 200 + m_random.nextInt(40);
    m_firePeriod = static_cast<int>(m_fireInterval / (1.2 + m_random.nextFloat(0.4)));
    if (rank < 10) {
        m_firePeriod /= (2 - rank * 0.1);
    }
}

void EnemyType::setLargeBossEnemyType(float rank, int mode) {
    m_type = LARGE;

    m_barragePatternSize = 2 + m_random.nextInt(3);
    m_wingCollision = true;

    //setEnemyColorType();

    int bn1 = 1 + m_random.nextInt(3);
    int bn2 = 1 + m_random.nextInt(3);

    for (int i = 0; i < m_barragePatternSize; ++i) {
        Barrage *barrage = &(m_barrage[i]);
        setBarrageType(barrage, BarrageManager::LARGE, mode);

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

        setBarrageRankSlow(barrage, cr, NORMAL, mode, 0.9);
        //setBarrageShape(barrage, 1.0);

        barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

        setEnemyShapeAndWings(LARGE_BOSS);
        setFormation(sr1, bn1, MEDIUM, NORMAL, 0, i, 0.9, mode);
        setFormation(sr2, bn2, MEDIUM, NORMAL, 2, i, 0.9, mode);
    }

    m_shield = 400 + m_random.nextInt(50);
    m_fireInterval = 220 + m_random.nextInt(60);
    m_firePeriod = static_cast<int>(m_fireInterval / (1.2 + m_random.nextFloat(0.3)));
    if (rank < 10) {
        m_firePeriod /= (2 - rank * 0.1);
    }
}

void EnemyType::setBarrageType(Barrage *barrage, int btn, int mode) {
    barrage->m_parser = reinterpret_cast<BulletMLParser *>(
        m_barrageManager->m_parser[btn][m_random.nextInt(m_barrageManager->m_parserSizes[btn])]);
    
    for (int i = 0; i < BarrageManager::BARRAGE_MAX; ++i) {
        m_usedMorphParser[i] = false;
    }

    int mpn;
    mpn = m_barrageManager->m_parserSizes[BarrageManager::MORPH];
    //mpn = m_barrageManager->m_parserSizes[BarrageManager::MORPH_LOCK];

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

        barrage->m_morphParser[i] = reinterpret_cast<BulletMLParser *>(
            m_barrageManager->m_parser[BarrageManager::MORPH][mi]);
        //barrage->m_morphParser[i] = reinterpret_cast<BulletMLParser *>(
        //    m_barrageManager->m_parser[BarrageManager::MORPH_LOCK][mi]);

        m_usedMorphParser[mi] = true;
    }

    barrage->m_morphSize = barrage->m_morphParser.size();
}

void EnemyType::setBarrageRank(Barrage *barrage, float rank, int intensity, int mode) {
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
    //barrage->m_speedRank = sqrt(rank * 0.66) * (m_random.nextFloat(0.2) + 0.8);
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

void EnemyType::setBarrageRankSlow(Barrage *barrage, float rank, int intensity, int mode, float slowness) {
    setBarrageRank(barrage, rank, intensity, mode);
    barrage->m_speedRank *= slowness;
}

void EnemyType::setEnemyShapeAndWings(int size) {
    float x1 = m_enemySize[size][0] + m_random.nextSignedFloat(m_enemySize[size][1]);
    float y1 = m_enemySize[size][2] + m_random.nextSignedFloat(m_enemySize[size][3]);
    float x2 = m_enemySize[size][0] + m_random.nextSignedFloat(m_enemySize[size][1]);
    float y2 = m_enemySize[size][2] + m_random.nextSignedFloat(m_enemySize[size][3]);
    m_bodyShapePosition[0].X = -x1;
    m_bodyShapePosition[0].Y = y1;
    m_bodyShapePosition[1].X = x1;
    m_bodyShapePosition[1].Y = y1;
    m_bodyShapePosition[2].X = x2;
    m_bodyShapePosition[2].Y = -y2;
    m_bodyShapePosition[3].X = -x2;
    m_bodyShapePosition[3].Y = -y2;

    //m_retroSize = m_enemySize[size][4];
    switch (size) {
    case SMALL:
    case MEDIUM:
    case MEDIUM_BOSS:
        m_formationSize = 2;
        break;
    case LARGE:
    case LARGE_BOSS:
        m_formationSize = 4;
        break;
    }

    float px = 0;
    float py = 0;
    float mpx = 0;
    float mpy = 0;
    int bsl = 0;
    if (x1 > x2) {
        m_collisionSize.X = x1;
    } else {
        m_collisionSize.X = x2;
    }
    if (y1 > y2) {
        m_collisionSize.Y = y1;
    } else {
        m_collisionSize.Y = y2;
    }

    for (int i = 0; i < m_formationSize; ++i) {
        EnemyFormationType *bt = &(m_formationType[i]);
        int wrl = 1;
        if (i % 2 == 0) {
            px = m_enemySize[size][5] + m_random.nextFloat(m_enemySize[size][6]);
            if (m_formationSize <= 2) {
                py = m_random.nextSignedFloat(m_enemySize[size][7]);
            } else {
                if (i < 2) {
                    py = m_random.nextFloat(m_enemySize[size][7] / 2) + m_enemySize[size][7] / 2;
                } else {
                    py = -m_random.nextFloat(m_enemySize[size][7] / 2) - m_enemySize[size][7] / 2;
                }
            }

            float md;
            if (m_random.nextInt(2) == 0) {
                md = m_random.nextFloat(M_PI / 2) - M_PI / 4;
            } else {
                md = m_random.nextFloat(M_PI / 2) + M_PI / 4 * 3;
            }
            mpx = px / 2 + sin(md) * (m_enemySize[size][8] / 2 + m_random.nextFloat(m_enemySize[size][8] / 2));
            mpy = py / 2 + cos(md) * (m_enemySize[size][8] / 2 + m_random.nextFloat(m_enemySize[size][8] / 2));

            switch (size) {
            case SMALL:
            case MEDIUM:
            case LARGE:
                bsl = 1;
                break;
            case MEDIUM_BOSS:
                bsl = 150 + m_random.nextInt(30);
                break;
            case LARGE_BOSS:
                bsl = 200 + m_random.nextInt(50);
                break;
            }

            wrl = -1;
            if (!m_wingCollision) {
                if (px > m_collisionSize.X) {
                    m_collisionSize.X = px;
                }

                float cpy = fabs(py);
                if (cpy > m_collisionSize.Y) {
                    m_collisionSize.Y = cpy;
                }

                cpy = fabs(mpy);
                if (cpy > m_collisionSize.Y) {
                    m_collisionSize.Y = cpy;
                }
            }
        }

        switch (wrl) {
        case 1:
            bt->m_wingShapePosition[0].X = px / 4 * wrl;
            bt->m_wingShapePosition[0].Y = py / 4;
            bt->m_wingShapePosition[1].X = px * wrl;
            bt->m_wingShapePosition[1].Y = py;
            bt->m_wingShapePosition[2].X = mpx * wrl;
            bt->m_wingShapePosition[2].Y = mpy;
            break;
        case -1:
            bt->m_wingShapePosition[0].X = px / 4 * wrl;
            bt->m_wingShapePosition[0].Y = py / 4;
            bt->m_wingShapePosition[1].X = px * wrl;
            bt->m_wingShapePosition[1].Y = py;
            bt->m_wingShapePosition[2].X = mpx * wrl;
            bt->m_wingShapePosition[2].Y = mpy;
            break;
        }

        bt->m_collisionPosition.X = (px + px / 4) / 2 * wrl;
        bt->m_collisionPosition.Y = (py + mpy + py / 4) / 3;
        bt->m_collisionSize.X = px / 4 * 3 / 2;

        float sy1 = fabs(py - mpy) / 2;
        float sy2 = fabs(py - py / 4) / 2;
        if (sy1 > sy2) {
            bt->m_collisionSize.Y = sy1;
        } else {
            bt->m_collisionSize.Y = sy2;
        }

        //bt->m_r = m_er;
        //bt->m_g = m_eg;
        //bt->m_b = m_eb;
        bt->m_shield = bsl;
    }
}

void EnemyType::setFormation(float rank, int n, int barrageType, int barrageIntensity, int idx, int ptnIdx, float slowness, int mode) {
    EnemyFormationType *bt = &(m_formationType[idx]);
    EnemyFormationType *bt2 = &(m_formationType[idx + 1]);
    Barrage *barrage = &(bt->m_barrage[ptnIdx]);
    Barrage *barrage2 = &(bt2->m_barrage[ptnIdx]);

    setBarrageType(barrage, barrageType, mode);
    setBarrageRankSlow(barrage, rank / n, barrageIntensity, mode, slowness);
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