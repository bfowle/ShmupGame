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
    BulletMLParser * m_parser;
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
    std::array<FVector2D, WING_FORMATION_MAX> m_formationPosition;
    std::array<FVector2D, WING_SHAPE_POINT_NUM> m_wingShapePosition;
    int m_formationSize;
    int m_shield;

    FVector2D m_collisionPosition;
    FVector2D m_collisionSize;
    bool m_xReverseAlternate;
};

class EnemyType {
public:
    EnemyType();

    static void init(std::shared_ptr<BarrageManager> barrageManager);
    static void clearExistsList();

    void setSmallEnemyType(float rank, int mode);
    void setMediumEnemyType(float rank, int mode);
    void setLargeEnemyType(float rank, int mode);
    void setMediumBossEnemyType(float rank, int mode);
    void setLargeBossEnemyType(float rank, int mode);

private:
    void setBarrageType(Barrage *barrage, int btn, int mode);
    void setBarrageRank(Barrage *barrage, float rank, int intensity, int mode);
    void setBarrageRankSlow(Barrage *barrage, float rank, int intensity, int mode, float slowness);
    void setEnemyShapeAndWings(int size);
    void setFormation(float rank, int n, int barrageType, int barrageIntensity, int idx, int ptnIdx, float slowness, int mode);

public:
    enum {
        BARRAGE_PATTERN_MAX = EnemyFormationType::BARRAGE_PATTERN_MAX,
        BODY_SHAPE_POINT_NUM = 4,
        FORMATION_MAX = 4,
        ENEMY_TYPE_MAX = 32,
        BULLET_SHAPE_NUM = 7,
        BULLET_COLOR_NUM = 4
    };

    enum {
        SMALL,
        MEDIUM,
        LARGE,
        MEDIUM_BOSS,
        LARGE_BOSS,
    };

    int m_id;
    int m_type;
    std::array<EnemyFormationType, FORMATION_MAX> m_formationType;
    int m_formationSize;
    int m_fireInterval, m_firePeriod, m_barragePatternSize;
    int m_shield;

    static std::array<bool, ENEMY_TYPE_MAX> m_exists;
    std::array<Barrage, BARRAGE_PATTERN_MAX> m_barrage;
    std::array<FVector2D, BODY_SHAPE_POINT_NUM> m_bodyShapePosition;

    FVector2D m_collisionSize;
    bool m_wingCollision;

private:
    enum {
        NORMAL,
        WEAK,
        VERY_WEAK,
        MORPH_WEAK
    };

    static Random m_random;
    static std::shared_ptr<BarrageManager> m_barrageManager;
    static std::array<bool, BarrageManager::BARRAGE_MAX> m_usedMorphParser;
    static int m_idCnt;
    static const float m_enemySize[][9];
};

#endif