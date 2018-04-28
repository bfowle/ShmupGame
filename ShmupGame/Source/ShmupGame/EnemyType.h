#ifndef ENEMYTYPE_H
#define ENEMYTYPE_H

#include "BarrageManager.h"
#include "MorphBullet.h"
#include "Random.h"

#include "Math/Vector.h"

#include <array>
#include <memory>

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

    int m_morphSize, m_morphCnt;
    float m_rank, m_speedRank, m_morphRank;
    float m_xReverse;
};

class EnemyFormationType {
public:
    enum {
        WING_SHAPE_POINT_NUM = 3,
        WING_FORMATION_MAX = 3,
        BARRAGE_PATTERN_MAX = 8
    };

    std::array<Barrage, BARRAGE_PATTERN_MAX> m_barrage;
    std::array<FVector, WING_FORMATION_MAX> m_formationPosition;
    std::array<FVector, WING_SHAPE_POINT_NUM> m_wingShapePosition;
    int m_formationSize;
    int m_shield;

    FVector m_collisionPosition;
    FVector m_collisionSize;
    bool m_xReverseAlternate;
};

class EnemyType {
public:
    EnemyType();

    static void init(std::shared_ptr<BarrageManager> barrageManager);
    static void clearExistsList();

    void setSmallEnemyType(float rank);
    void setMediumEnemyType(float rank);
    void setLargeEnemyType(float rank);
    void setMediumBossEnemyType(float rank);
    void setLargeBossEnemyType(float rank);

private:
    void setBarrageType(Barrage *barrage, int btn);
    void setBarrageRank(Barrage *barrage, float rank, int intensity);
    void setBarrageRankSlow(Barrage *barrage, float rank, int intensity, float slowness);
    void setFormation(float rank, int n, int barrageType, int barrageIntensity, int idx, int ptnIdx, float slowness);

public:
    enum {
        BARRAGE_PATTERN_MAX = EnemyFormationType::BARRAGE_PATTERN_MAX,
        FORMATION_MAX = 4,
        ENEMY_TYPE_MAX = 32
    };

    enum { SMALL, MEDIUM, LARGE, MEDIUM_BOSS, LARGE_BOSS, };

    int m_id;
    int m_type;
    std::array<EnemyFormationType, FORMATION_MAX> m_formationType;
    int m_formationSize;
    int m_barragePatternSize;
    int m_shield;

    static std::array<bool, ENEMY_TYPE_MAX> m_exists;
    std::array<Barrage, BARRAGE_PATTERN_MAX> m_barrage;

private:
    enum { NORMAL, WEAK, VERY_WEAK, MORPH_WEAK };

    static Random m_random;
    static std::shared_ptr<BarrageManager> m_barrageManager;
    static std::array<bool, BarrageManager::BARRAGE_MAX> m_usedMorphParser;
    static int m_idCnt;
};

#endif