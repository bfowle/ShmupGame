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

class StageManager {
public:
    struct EnemySquadron {
    public:
        std::shared_ptr<EnemyType> m_type;
        BulletMLParser *m_moveParser;
        int m_id;
        float m_position;
        int m_point, m_pattern, m_sequence, m_side;
        int m_total, m_cnt, m_remaining;
    };

    void init(std::shared_ptr<BarrageManager> barrageManager, TWeakObjectPtr<AGameManager> gameManager);
    void setRank(float baseRank, float inc, int type);

private:
    void createSectionData();
    void createStage();
    void createEnemyData();
    void setSquadronPattern(EnemySquadron *squadron);
    void setSquadron(EnemySquadron *squadron, int type);
    void setSmallSquadron(EnemySquadron *squadron);
    void setMediumSquadron(EnemySquadron *squadron);
    void setLargeSquadron(EnemySquadron *squadron);

public:
    enum {
        STAGE_TYPE_NUM = 4,
        SIMULTANEOUS_APPEARANCE_MAX = 4,
        SMALL_ENEMY_TYPE_MAX = 3,
        MEDIUM_ENEMY_TYPE_MAX = 4,
        LARGE_ENEMY_TYPE_MAX = 2
    };

    enum { TOP, SIDE, BACK };
    enum { ONE_SIDE, ALTERNATE, BOTH_SIDES };
    enum { RANDOM, FIXED };
    enum { SMALL, MEDIUM, LARGE };

    std::array<EnemySquadron, SIMULTANEOUS_APPEARANCE_MAX> m_squadrons;

private:
    Random m_random;
    TWeakObjectPtr<AGameManager> m_gameManager;
    std::shared_ptr<BarrageManager> m_barrageManager;

    static const int m_squadronPatterns[7][3];

    std::array<std::shared_ptr<EnemyType>, SMALL_ENEMY_TYPE_MAX> m_smallType;
    std::array<std::shared_ptr<EnemyType>, MEDIUM_ENEMY_TYPE_MAX> m_mediumType;
    std::array<std::shared_ptr<EnemyType>, LARGE_ENEMY_TYPE_MAX> m_largeType;
    std::shared_ptr<EnemyType> m_mediumBossType;
    std::shared_ptr<EnemyType> m_largeBossType;

    int m_squadronTotal;
    FVector2D m_squadronPosition;
    int m_rank, m_rankInc;
    int m_stageType;
};

#endif