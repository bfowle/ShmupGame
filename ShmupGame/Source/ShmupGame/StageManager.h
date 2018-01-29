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
    void init(std::shared_ptr<Field> field, std::shared_ptr<BarrageManager> barrageManager, AGameManager *gameManager);
    void setRank(float baseRank, float inc, int startParsec, int type);
    void tick();

private:
    struct EnemyAppearance {
    public:
        std::shared_ptr<EnemyType> m_type;
        BulletMLParser *m_moveParser;
        int m_point, m_pattern, m_sequence;
        float m_position;
        int m_num, m_interval, m_groupInterval;
        int m_cnt, m_left, m_side;
    };

    void createEnemyData();
    void setAppearancePattern(EnemyAppearance *appearance);
    void setSmallAppearance(EnemyAppearance *appearance);
    void setMediumAppearance(EnemyAppearance *appearance);
    void setLargeAppearance(EnemyAppearance *appearance);
    void setAppearance(EnemyAppearance *appearance, int type);
    void createSectionData();
    void createStage();
    void gotoNextSection();

public:
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

    enum {
        STAGE_TYPE_NUM = 4,
        SIMULTANEOUS_APPEARANCE_MAX = 4,
        SMALL_ENEMY_TYPE_MAX = 3,
        MEDIUM_ENEMY_TYPE_MAX = 4,
        LARGE_ENEMY_TYPE_MAX = 2
    };

    int m_parsec;
    bool m_bossSection;

private:
    enum {
        MEDIUM_RUSH_SECTION_PATTERN = 6
    };

    Random m_random;
    AGameManager *m_gameManager;
    std::shared_ptr<BarrageManager> m_barrageManager;
    std::shared_ptr<Field> m_field;

    static const int m_appearancePattern[][7][3];

    std::array<EnemyAppearance, SIMULTANEOUS_APPEARANCE_MAX> m_appearance;
    std::array<std::shared_ptr<EnemyType>, SMALL_ENEMY_TYPE_MAX> m_smallType;
    std::array<std::shared_ptr<EnemyType>, MEDIUM_ENEMY_TYPE_MAX> m_mediumType;
    std::array<std::shared_ptr<EnemyType>, LARGE_ENEMY_TYPE_MAX> m_largeType;
    std::shared_ptr<EnemyType> m_mediumBossType;
    std::shared_ptr<EnemyType> m_largeBossType;

    int m_appearanceNum;
    FVector2D m_appearancePosition;
    int m_sectionCnt, m_sectionIntervalCnt, m_section;
    float m_rank, m_rankInc;
    int m_mediumRushSectionNum;
    bool m_mediumRushSection;
    int m_stageType;
};

#endif