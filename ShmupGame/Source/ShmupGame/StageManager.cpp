#include "StageManager.h"

#include "GameManager.h"
#include "Enemy.h"
#include "Field.h"

#include "Engine/World.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"

using namespace std;

const int StageManager::m_squadronPatterns[7][3] = {
    // 0: small | 1: medium | 2: large
    {1, 0, 0},
    {2, 0, 0},
    {1, 1, 0},
    {1, 0, 1},
    {2, 1, 0},
    {2, 0, 1},
    {0, 1, 1}
};

void StageManager::init(shared_ptr<BarrageManager> barrageManager, TWeakObjectPtr<AGameManager> gameManager) {
    m_barrageManager = barrageManager;
    m_gameManager = gameManager;

    m_random = Random();
    m_squadronPosition = FVector2D();

    for (int i = 0; i < m_smallType.size(); ++i) {
        m_smallType[i].reset(new EnemyType());
    }
    for (int i = 0; i < m_mediumType.size(); ++i) {
        m_mediumType[i].reset(new EnemyType());
    }
    for (int i = 0; i < m_largeType.size(); ++i) {
        m_largeType[i].reset(new EnemyType());
    }
    m_mediumBossType.reset(new EnemyType());
    m_largeBossType.reset(new EnemyType());
}

void StageManager::setRank(float baseRank, float inc, int type) {
    m_rank = baseRank;
    m_rankInc = inc;
    m_rank += m_rankInc;
    m_stageType = type;

    createStage();
    createSectionData();
}

void StageManager::createSectionData() {
    m_squadronTotal = 0;
    if (m_rank <= 0) {
        return;
    }

    int sp = 3 / 7 + 1;
    int ep = 3 + 3 / 10;
    int ap = sp + m_random.nextInt(ep - sp + 1);

    for (int i = 0; i < m_squadronPatterns[ap][0]; ++i, ++m_squadronTotal) {
        EnemySquadron *squadron = &m_squadrons[m_squadronTotal];
        setSquadron(squadron, SMALL);
    }
    for (int i = 0; i < m_squadronPatterns[ap][1]; ++i, ++m_squadronTotal) {
        EnemySquadron *squadron = &m_squadrons[m_squadronTotal];
        setSquadron(squadron, MEDIUM);
    }
    for (int i = 0; i < m_squadronPatterns[ap][2]; ++i, ++m_squadronTotal) {
        EnemySquadron *squadron = &m_squadrons[m_squadronTotal];
        setSquadron(squadron, LARGE);
    }
}

void StageManager::createStage() {
    createEnemyData();

    ++m_stageType;
}

void StageManager::createEnemyData() {
    for (int i = 0; i < m_smallType.size(); ++i) {
        m_smallType[i]->setSmallEnemyType(m_rank);
    }
    for (int i = 0; i < m_mediumType.size(); ++i) {
        m_mediumType[i]->setMediumEnemyType(m_rank);
    }
    for (int i = 0; i < m_largeType.size(); ++i) {
        m_largeType[i]->setLargeEnemyType(m_rank);
    }
    m_mediumBossType->setMediumBossEnemyType(m_rank);
    m_largeBossType->setMediumBossEnemyType(m_rank);
}

void StageManager::setSquadronPattern(EnemySquadron *squadron) {
    switch (m_random.nextInt(5)) {
    case 0:
        squadron->m_pattern = ONE_SIDE;
        break;
    case 1:
    case 2:
        squadron->m_pattern = ALTERNATE;
        break;
    case 3:
    case 4:
        squadron->m_pattern = BOTH_SIDES;
        break;
    }

    switch (m_random.nextInt(3)) {
    case 0:
        squadron->m_sequence = RANDOM;
        break;
    case 1:
    case 2:
        squadron->m_sequence = FIXED;
        break;
    }
}

static int squadronCnt = 0;

void StageManager::setSquadron(EnemySquadron *squadron, int type) {
    switch (type) {
    case SMALL:
        setSmallSquadron(squadron);
        break;
    case MEDIUM:
        setMediumSquadron(squadron);
        break;
    case LARGE:
        setLargeSquadron(squadron);
        break;
    }

    squadron->m_id = ++squadronCnt;
    squadron->m_cnt = 0;
    squadron->m_remaining = squadron->m_total;
    squadron->m_side = m_random.nextInt(2) * 2 - 1;
    squadron->m_position = m_random.nextFloat(1);
}

void StageManager::setSmallSquadron(EnemySquadron *squadron) {
    squadron->m_type = m_smallType[m_random.nextInt(m_smallType.size())];

    int mt;
    if (m_random.nextFloat(1) > 0.2) {
        squadron->m_point = TOP;
        mt = BarrageManager::SMALL_MOVE;
    } else {
        squadron->m_point = SIDE;
        mt = BarrageManager::SMALL_SIDE_MOVE;
    }

    //squadron->m_moveParser = m_barrageManager->m_parser[mt][m_random.nextInt(m_barrageManager->m_parserSizes[mt])];
    setSquadronPattern(squadron);

    if (squadron->m_pattern == ONE_SIDE) {
        squadron->m_pattern = ALTERNATE;
    }

    switch (m_random.nextInt(4)) {
    case 0:
        squadron->m_total = 7 + m_random.nextInt(5);
        break;
    case 1:
        squadron->m_total = 5 + m_random.nextInt(3);
        break;
    case 2:
    case 3:
        squadron->m_total = 2 + m_random.nextInt(2);
        break;
    }
}

void StageManager::setMediumSquadron(EnemySquadron *squadron) {
    squadron->m_type = m_mediumType[m_random.nextInt(m_mediumType.size())];

    int mt = BarrageManager::MEDIUM_MOVE;
    squadron->m_point = TOP;
    //squadron->m_moveParser = m_barrageManager->m_parser[mt][m_random.nextInt(m_barrageManager->m_parserSizes[mt])];
    setSquadronPattern(squadron);

    switch (m_random.nextInt(3)) {
    case 0:
        squadron->m_total = 4;
        break;
    case 1:
        squadron->m_total = 2;
        break;
    case 2:
        squadron->m_total = 1;
        break;
    }
}

void StageManager::setLargeSquadron(EnemySquadron *squadron) {
    squadron->m_type = m_largeType[m_random.nextInt(m_largeType.size())];

    int mt;
    squadron->m_point = TOP;
    mt = BarrageManager::LARGE_MOVE;

    //squadron->m_moveParser = m_barrageManager->m_parser[mt][m_random.nextInt(m_barrageManager->m_parserSizes[mt])];
    setSquadronPattern(squadron);

    switch (m_random.nextInt(3)) {
    case 0:
        squadron->m_total = 3;
        break;
    case 1:
        squadron->m_total = 2;
        break;
    case 2:
        squadron->m_total = 1;
        break;
    }
}