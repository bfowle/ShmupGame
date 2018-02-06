#ifndef STAGEMANAGER_H
#define STAGEMANAGER_H

#include "BarrageManager.h"
#include "EnemyType.h"
#include "Random.h"

#include <array>
#include <memory>
#include <vector>

class AGameManager;
class BulletMLParser;
class Field;

class StageManager {
public:
    void init(std::shared_ptr<Field> field, std::shared_ptr<BarrageManager> barrageManager, TWeakObjectPtr<AGameManager> gameManager);
    void setRank(float baseRank, float inc, int startParsec, int type);
    void tick();

private:
    struct EnemyFleet {
    public:
        std::shared_ptr<EnemyType> m_type;
        BulletMLParser *m_moveParser;
        int m_point, m_pattern, m_sequence;
        float m_position;
        int m_id;
        int m_side;
        int m_total, m_cnt, m_remaining;
        float m_interval, m_groupInterval;
    };

    void createSectionData();
    void createStage();
    void createEnemyData();
    void setFleetPattern(EnemyFleet *fleet);
    void setFleet(EnemyFleet *fleet, int type);
    void setSmallFleet(EnemyFleet *fleet);
    void setMediumFleet(EnemyFleet *fleet);
    void setLargeFleet(EnemyFleet *fleet);
    void gotoNextSection();

public:
    enum {
        STAGE_TYPE_NUM = 4,
        SIMULTANEOUS_APPEARANCE_MAX = 4,
        SMALL_ENEMY_TYPE_MAX = 3,
        MEDIUM_ENEMY_TYPE_MAX = 4,
        LARGE_ENEMY_TYPE_MAX = 2
    };

    enum {
        TOP,
        SIDE,
        BACK
    };

    enum {
        ONE_SIDE,
        ALTERNATE,
        BOTH_SIDES
    };

    enum {
        RANDOM,
        FIXED
    };

    enum {
        SMALL,
        MEDIUM,
        LARGE
    };

    int m_parsec;
    bool m_isBossSection;

private:
    enum {
        MEDIUM_RUSH_SECTION_PATTERN = 6
    };

    Random m_random;
    TWeakObjectPtr<AGameManager> m_gameManager;
    std::shared_ptr<BarrageManager> m_barrageManager;
    std::shared_ptr<Field> m_field;

    static const int m_fleetPatterns[][7][3];

    std::array<EnemyFleet, SIMULTANEOUS_APPEARANCE_MAX> m_fleets;
    std::array<std::shared_ptr<EnemyType>, SMALL_ENEMY_TYPE_MAX> m_smallType;
    std::array<std::shared_ptr<EnemyType>, MEDIUM_ENEMY_TYPE_MAX> m_mediumType;
    std::array<std::shared_ptr<EnemyType>, LARGE_ENEMY_TYPE_MAX> m_largeType;
    std::shared_ptr<EnemyType> m_mediumBossType;
    std::shared_ptr<EnemyType> m_largeBossType;

    float m_deltaSeconds;
    int m_fleetTotal;
    FVector2D m_fleetPosition;
    float m_sectionCnt, m_sectionIntervalCnt;
    int m_section;
    int m_rank, m_rankInc;
    int m_mediumRushSection;
    bool m_isMediumRushSection;
    int m_stageType;
};

#endif