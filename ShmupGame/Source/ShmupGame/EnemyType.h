#ifndef ENEMYTYPE_H
#define ENEMYTYPE_H

#include "BarrageManager.h"
#include "BatteryType.h"
#include "MorphBullet.h"
#include "Random.h"

#include "Math/Vector.h"

#include <memory>

class Barrage;

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
    void setBattery(float rank, int n, int barrageType, int barrageIntensity, int idx, int ptnIdx, float slowness, int mode);

public:
    enum {
        BARRAGE_PATTERN_MAX = BatteryType::BARRAGE_PATTERN_MAX,
        BODY_SHAPE_POINT_NUM = 4,
        BATTERY_MAX = 4,
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

    enum {
        ROLL,
        LOCK
    };

    int m_id;
    int m_type;
    std::array<BatteryType, BATTERY_MAX> m_batteryType;
    int m_batterySize;
    int m_fireInterval, m_firePeriod, m_barragePatternSize;
    int m_shield;

    std::array<Barrage, BARRAGE_PATTERN_MAX> m_barrage;
    static std::array<bool, ENEMY_TYPE_MAX> m_exists;
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
    static int m_idCnt;
    static std::shared_ptr<BarrageManager> m_barrageManager;
    static std::array<bool, BarrageManager::BARRAGE_MAX> m_usedMorphParser;

    //float er, eg, eb;
    int m_enemyColorType;
    static const float m_enemySize[][9];
};

#endif