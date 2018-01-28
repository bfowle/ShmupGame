#include "EnemyType.h"

#include "BarrageManager.h"

using namespace std;

array<bool, EnemyType::ENEMY_TYPE_MAX> EnemyType::m_exists;

Random EnemyType::m_random;
int EnemyType::m_idCnt;
shared_ptr<BarrageManager> EnemyType::m_barrageManager(new BarrageManager());
array<bool, BarrageManager::BARRAGE_MAX> EnemyType::m_usedMorphParser;

const float EnemyType::m_enemySize[][9] = {
    { 0.3, 0.3, 0.3, 0.1, 0.1,  1.0, 0.4, 0.6, 0.9 },
    { 0.4, 0.2, 0.4, 0.1, 0.15, 2.2, 0.2, 1.6, 1.0 },
    { 0.6, 0.3, 0.5, 0.1, 0.2,  3.0, 0.3, 1.4, 1.2 },
    { 0.9, 0.3, 0.7, 0.2, 0.25, 5.0, 0.6, 3.0, 1.5 },
    { 1.2, 0.2, 0.9, 0.1, 0.3,  7.0, 0.8, 4.5, 1.5 },
};

EnemyType::EnemyType() :
    m_id(m_idCnt) {
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
    m_barragePatternNum = 1;
    //m_wingCollision = false;

    setEnemyColorType();

    Barrage *barrage = &(m_barrage[0]);
    if (mode == ROLL) {
        setBarrageType(barrage, BarrageManager::SMALL, mode);
    } else {
        setBarrageType(barrage, BarrageManager::SMALL_LOCK, mode);
    }
    setBarrageRank(barrage, rank, VERY_WEAK, mode);
    setBarrageShape(barrage, 0.7);

    barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

    setEnemyShapeAndWings(SMALL);
    setBattery(0, 0, 0, NORMAL, 0, 0, 1, mode);

    m_shield = 1;
    m_fireInterval = 99999;
    m_firePeriod = 150 + m_random.nextInt(40);
    if (rank < 10) {
        m_firePeriod /= (2 - rank * 0.1);
    }
}

void EnemyType::setMediumEnemyType(float rank, int mode) {
    m_type = MEDIUM;
    m_barragePatternNum = 1;
    //m_wingCollision = false;
    
    setEnemyColorType();

    Barrage *barrage = &(m_barrage[0]);
    setBarrageType(barrage, BarrageManager::MEDIUM, mode);

    float cr = 0;
    float sr = 0;
    if (mode == ROLL) {
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
    } else {
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
    }

    setBarrageRank(barrage, cr, MORPH_WEAK, mode);
    setBarrageShape(barrage, 0.75);

    barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

    setEnemyShapeAndWings(MEDIUM);

    if (mode == ROLL) {
        m_shield = 40 + m_random.nextInt(10);
        setBattery(sr, 1, BarrageManager::MEDIUM_SUB, NORMAL, 0, 0, 1, mode);
        m_fireInterval = 72 + m_random.nextInt(60);
        m_firePeriod = static_cast<int>(m_fireInterval / (1.8 + m_random.nextFloat(0.7)));
    } else {
        m_shield = 30 + m_random.nextInt(8);
        m_fireInterval = 72 + m_random.nextInt(30);
        m_firePeriod = static_cast<int>(m_fireInterval / 1.2 + m_random.nextFloat(0.2));
    }

    if (rank < 10) {
        m_firePeriod /= (2 - rank * 0.1);
    }
}

void EnemyType::setLargeEnemyType(float rank, int mode) {
    m_type = LARGE;

    m_barragePatternNum = 2 + m_random.nextInt(3);
    //m_wingCollision = true;

    setEnemyColorType();

    int bn1 = 1 + m_random.nextInt(3);
    int bn2 = 1 + m_random.nextInt(3);

    for (int i = 0; i < m_barragePatternNum; i++) {
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
        setBarrageShape(barrage, 1.0);

        barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

        setEnemyShapeAndWings(LARGE_BOSS);
        setBattery(sr1, bn1, BarrageManager::MEDIUM, NORMAL, 0, i, 0.9, mode);
        setBattery(sr2, bn2, BarrageManager::MEDIUM, NORMAL, 2, i, 0.9, mode);
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

    m_barragePatternNum = 2 + m_random.nextInt(2);
    //m_wingCollision = true;

    setEnemyColorType();

    int bn = 1 + m_random.nextInt(2);

    for (int i = 0; i < m_barragePatternNum; ++i) {
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
        setBarrageShape(barrage, 0.9);

        barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

        setEnemyShapeAndWings(MEDIUM_BOSS);
        setBattery(sr, bn, MEDIUM, WEAK, 0, i, 0.9, mode);
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

    m_barragePatternNum = 2 + m_random.nextInt(3);
    //m_wingCollision = true;

    setEnemyColorType();

    int bn1 = 1 + m_random.nextInt(3);
    int bn2 = 1 + m_random.nextInt(3);

    for (int i = 0; i < m_barragePatternNum; ++i) {
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
        setBarrageShape(barrage, 1.0);

        barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

        setEnemyShapeAndWings(LARGE_BOSS);
        setBattery(sr1, bn1, MEDIUM, NORMAL, 0, i, 0.9, mode);
        setBattery(sr2, bn2, MEDIUM, NORMAL, 2, i, 0.9, mode);
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
        m_barrageManager->m_parser[btn][m_random.nextInt(m_barrageManager->m_parserNum[btn])]);
    
    for (int i = 0; i < BarrageManager::BARRAGE_MAX; ++i) {
        m_usedMorphParser[i] = false;
    }

    int mpn;

    if (mode == ROLL) {
        mpn = m_barrageManager->m_parserNum[BarrageManager::MORPH];
    } else {
        mpn = m_barrageManager->m_parserNum[BarrageManager::MORPH_LOCK];
    }

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

        if (mode == ROLL) {
            barrage->m_morphParser[i] = reinterpret_cast<BulletMLParser *>(
                m_barrageManager->m_parser[BarrageManager::MORPH][mi]);
        } else {
            barrage->m_morphParser[i] = reinterpret_cast<BulletMLParser *>(
                m_barrageManager->m_parser[BarrageManager::MORPH_LOCK][mi]);
        }

        m_usedMorphParser[mi] = true;
    }

    barrage->m_morphNum = barrage->m_morphParser.size();
}

void EnemyType::setBarrageRank(Barrage *barrage, float rank, int intensity, int mode) {
    if (rank <= 0) {
        barrage->m_rank = 0;
        return;
    }

    barrage->m_rank = sqrt(rank) / (8 - m_random.nextInt(3));
    if (barrage->m_rank > 0.8) {
        barrage->m_rank = m_random.nextFloat(0.2) + 0.8;
    }
    rank /= (barrage->m_rank + 2);
    if (intensity == WEAK) {
        barrage->m_rank /= 2;
    }

    if (mode == ROLL) {
        barrage->m_speedRank = sqrt(rank) * (m_random.nextFloat(0.2) + 1);
    } else {
        barrage->m_speedRank = sqrt(rank * 0.66) * (m_random.nextFloat(0.2) + 0.8);
    }
    if (barrage->m_speedRank < 1) {
        barrage->m_speedRank = 1;
    }
    if (barrage->m_speedRank > 2) {
        barrage->m_speedRank = sqrt(barrage->m_speedRank) + 0.27;
    }

    barrage->m_morphRank = rank / barrage->m_speedRank;
    barrage->m_morphCnt = 0;
    while (barrage->m_morphRank > 1) {
        barrage->m_morphCnt++;
        barrage->m_morphRank /= 3;
    }

    if (intensity == VERY_WEAK) {
        barrage->m_morphRank /= 2;
        barrage->m_morphCnt /= 1.7;
    } else if (intensity == MORPH_WEAK) {
        barrage->m_morphRank /= 2;
    } else if (intensity == WEAK) {
        barrage->m_morphRank /= 1.5;
    }
}

void EnemyType::setBarrageRankSlow(Barrage *barrage, float rank, int intensity, int mode, float slowness) {
    setBarrageRank(barrage, rank, intensity, mode);
    barrage->m_speedRank *= slowness;
}

void EnemyType::setBarrageShape(Barrage *barrage, float size) {
    /*
    barrage->m_shape = m_random.nextInt(BULLET_SHAPE_NUM);
    barrage->m_color = m_random.nextInt(BULLET_COLOR_NUM);
    barrage->m_bulletSize = (1.0 + m_random.nextSignedFloat(0.1)) * size;
    */
}

void EnemyType::setEnemyColorType() {
    m_enemyColorType = m_random.nextInt(3);
}

void EnemyType::createEnemyColor() {
    /*
    switch (m_enemyColorType) {
    case 0:
        er = 1;
        eg = m_random.nextFloat(0.7) + 0.3;
        eb = m_random.nextFloat(0.7) + 0.3;
        break;
    case 1:
        er = m_random.nextFloat(0.7) + 0.3;
        eg = 1;
        eb = m_random.nextFloat(0.7) + 0.3;
        break;
    case 2:
        er = m_random.nextFloat(0.7) + 0.3;
        eg = m_random.nextFloat(0.7) + 0.3;
        eb = 1;
        break;
    }
    */
}

void EnemyType::setEnemyShapeAndWings(int size) {

}

void EnemyType::setBattery(float rank, int n, int barrageType, int barrageIntensity, int idx, int ptnIdx, float slowness, int mode) {
    BatteryType *bt = &(m_batteryType[idx]);
    BatteryType *bt2 = &(m_batteryType[idx + 1]);
    Barrage *barrage = &(bt->m_barrage[ptnIdx]);
    Barrage *barrage2 = &(bt2->m_barrage[ptnIdx]);

    setBarrageType(barrage, barrageType, mode);
    setBarrageRankSlow(barrage, rank / n, barrageIntensity, mode, slowness);
    setBarrageShape(barrage, 0.8);

    barrage->m_xReverse = m_random.nextInt(2) * 2 - 1;

    barrage2->m_parser = barrage->m_parser;
    for (int i = 0; i < MorphBullet::MORPH_MAX; ++i) {
        barrage2->m_morphParser[i] = barrage->m_morphParser[i];
    }

    barrage2->m_morphNum = barrage->m_morphNum;
    barrage2->m_morphCnt = barrage->m_morphCnt;
    barrage2->m_rank = barrage->m_rank;
    barrage2->m_speedRank = barrage->m_speedRank;
    barrage2->m_morphRank = barrage->m_morphRank;
    //barrage2->m_shape = barrage->m_shape;
    //barrage2->m_color = barrage->m_color;
    barrage2->m_bulletSize = barrage->m_bulletSize;
    barrage2->m_xReverse = -barrage->m_xReverse;
    
    /*
    if (m_random.nextInt(4) == 0) {
        bt->m_xReverseAlternate = true;
        bt2->m_xReverseAlternate = true;
    } else {
        bt->m_xReverseAlternate = false;
        bt2->m_xReverseAlternate = false;
    }

    float px = bt->m_wingShapePos[1].X;
    float py = bt->m_wingShapePos[1].Z;
    float mpx = bt->m_wingShapePos[2].X;
    float mpy = bt->m_wingShapePos[2].Z;
    for (int i = 0; i < n; i++) {
        bt->m_batteryPos[i].X = px;
        bt->m_batteryPos[i].Z = py;
        bt2->m_batteryPos[i].X = -px;
        bt2->m_batteryPos[i].Z = py;
        px += (mpx - px) / (n - 1);
        py += (mpy - py) / (n - 1);
    }
    */

    bt->m_batteryNum = n;
    bt2->m_batteryNum = n;
}