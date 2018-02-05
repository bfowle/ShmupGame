#include "StageManager.h"

#include "GameManager.h"
#include "Enemy.h"
#include "Field.h"

#include "Engine/World.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"

using namespace std;

const int StageManager::m_fleetPatterns[][7][3] = {
    // 0: roll | 1: lock
    {
        // 0: small | 1: medium | 2: large
        {1, 0, 0},
        {2, 0, 0},
        {1, 1, 0},
        {1, 0, 1},
        {2, 1, 0},
        {2, 0, 1},
        {0, 1, 1}
    }, {
        {1, 0, 0},
        {1, 1, 0},
        {1, 1, 0},
        {1, 0, 1},
        {2, 1, 0},
        {1, 1, 1},
        {0, 1, 1}
    }
};

void StageManager::init(shared_ptr<Field> field, shared_ptr<BarrageManager> barrageManager, TWeakObjectPtr<AGameManager> gameManager) {
    m_field = field;
    m_barrageManager = barrageManager;
    m_gameManager = gameManager;

    m_random = Random();
    m_fleetPosition = FVector2D();

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

void StageManager::setRank(float baseRank, float inc, int startParsec, int type) {
    m_rank = baseRank;
    m_rankInc = inc;
    m_rank += m_rankInc * (startParsec / 10);
    m_section = -1;
    m_parsec = startParsec - 1;
    m_stageType = type;

    createStage();
    gotoNextSection();
}

void StageManager::tick() {
    m_deltaSeconds = m_gameManager->m_world->GetDeltaSeconds();

    for (int i = 0; i < m_fleetTotal; ++i) {
        EnemyFleet *fleet = &m_fleets[i];

        //fleet->m_cnt--;
        fleet->m_cnt -= 10.0 * m_gameManager->m_deltaSeconds;
        //UE_LOG(LogTemp, Warning, TEXT(" 1.) [%d] tick::cnt [%f] -- (%f) "), m_fleetTotal, fleet->m_cnt, m_gameManager->m_deltaSeconds);

        if (fleet->m_cnt > 0) {
            // add an extra enemy
            if (!m_isMediumRushSection) {
                if (fleet->m_type->m_type == EnemyType::SMALL &&
                    !EnemyType::m_exists[fleet->m_type->m_id]) {
                    //UE_LOG(LogTemp, Warning, TEXT(" 3-1.) tick::add::cnt [%f] "), fleet->m_cnt);
                    fleet->m_cnt = 0;
                    EnemyType::m_exists[fleet->m_type->m_id] = true;
                }
            } else {
                if (fleet->m_type->m_type == EnemyType::MEDIUM &&
                    !EnemyType::m_exists[fleet->m_type->m_id]) {
                    //UE_LOG(LogTemp, Warning, TEXT(" 3-2.) tick::add::cnt [%f] "), fleet->m_cnt);
                    fleet->m_cnt = 0;
                    EnemyType::m_exists[fleet->m_type->m_id] = true;
                }
            }
            continue;
        }

        float pos = 0;
        switch (fleet->m_sequence) {
        case RANDOM:
            pos = m_random.nextFloat(1);
            break;
        case FIXED:
            pos = fleet->m_position;
            break;
        }

        float direction = 0;
        switch (fleet->m_point) {
        case TOP:
            switch (fleet->m_pattern) {
            case BOTH_SIDES:
                m_fleetPosition.X = (pos - 0.5) * m_field->m_size.X * 1.8;
                break;
            default:
                m_fleetPosition.X = (pos * 0.6 + 0.2) * m_field->m_size.X * fleet->m_side;
                break;
            }
            m_fleetPosition.Y = m_field->m_size.Y - Enemy::FIELD_SPACE;
            direction = M_PI;
            break;

        case BACK:
            switch (fleet->m_pattern) {
            case BOTH_SIDES:
                m_fleetPosition.X = (pos - 0.5) * m_field->m_size.X * 1.8;
                break;
            default:
                m_fleetPosition.X = (pos * 0.6 + 0.2) * m_field->m_size.X * fleet->m_side;
                break;
            }
            m_fleetPosition.Y = -m_field->m_size.Y + Enemy::FIELD_SPACE;
            direction = 0;
            break;

        case SIDE:
            switch (fleet->m_pattern) {
            case BOTH_SIDES:
                m_fleetPosition.X = (m_field->m_size.X - Enemy::FIELD_SPACE) * (m_random.nextInt(2) * 2 - 1);
                break;
            default:
                m_fleetPosition.X = (m_field->m_size.X - Enemy::FIELD_SPACE) * fleet->m_side;
                break;
            }
            m_fleetPosition.Y = (pos * 0.4 + 0.4) * m_field->m_size.Y;
            if (m_fleetPosition.X < 0) {
                direction = M_PI / 2.0;
            } else {
                direction = M_PI / 2.0 * 3.0;
            }
            break;
        }
        m_fleetPosition.X *= 0.88;

        m_gameManager->addEnemy(m_fleetPosition, direction, fleet->m_type, fleet->m_moveParser);

        fleet->m_remaining--;
        if (fleet->m_remaining <= 0) {
            //UE_LOG(LogTemp, Warning, TEXT(" 2-1.) cnt=groupInterval [%f, %f] "), fleet->m_cnt, fleet->m_groupInterval);
            fleet->m_cnt = fleet->m_groupInterval;
            fleet->m_remaining = fleet->m_total;
            if (fleet->m_pattern != ONE_SIDE) {
                fleet->m_side *= -1;
            }
            fleet->m_position = m_random.nextFloat(1);
        } else {
            //UE_LOG(LogTemp, Warning, TEXT(" 2-2.) cnt=interval [%f, %f] "), fleet->m_cnt, fleet->m_interval);
            fleet->m_cnt = fleet->m_interval;
        }
    }

    if (!m_bossSection ||
        (!EnemyType::m_exists[m_mediumBossType->m_id] &&
         !EnemyType::m_exists[m_largeBossType->m_id])) {
        //--m_sectionCnt;
        m_sectionCnt -= 10.0 * m_deltaSeconds;
        //UE_LOG(LogTemp, Warning, TEXT(" [*] sectionCnt: %f ... [< sectionInt? => %f ] "), m_sectionCnt, m_sectionIntervalCnt);
    }

    if (m_sectionCnt < m_sectionIntervalCnt) {
        if (m_section == 9 &&
            m_sectionCnt == m_sectionIntervalCnt - 1) {
            // do stuff
        }
        m_fleetTotal = 0;

        if (m_sectionCnt <= 0) {
            gotoNextSection();
        }
    }

    EnemyType::clearExistsList();
}

void StageManager::createSectionData() {
    m_fleetTotal = 0;
    if (m_rank <= 0) {
        return;
    }

    m_field->m_aimSpeed = 0.1 + m_section * 0.02;

    UE_LOG(LogTemp, Warning, TEXT("!!!!!!!!!! StageManager::createSectionData => %d"), m_section);

    if (m_section == 4) {
        // set the medium boss
        FVector2D pos(0, m_field->m_size.Y / 4 * 3);

        m_gameManager->addBoss(pos, M_PI, m_mediumBossType);
        m_bossSection = true;

        m_sectionIntervalCnt = 2 * 60;
        m_sectionCnt = 2 * 60;

        m_field->m_aimZ = 11;
        return;
    } else if (m_section == 9) {
        // set the large boss
        FVector2D pos(0, m_field->m_size.Y / 4 * 3);

        m_gameManager->addBoss(pos, M_PI, m_largeBossType);
        m_bossSection = true;

        m_sectionIntervalCnt = 3 * 60;
        m_sectionCnt = 3 * 60;

        m_field->m_aimZ = 12;
        return;
    } else if (m_section == m_mediumRushSection) {
        // no small enemies in this section
        m_isMediumRushSection = true;
        m_field->m_aimZ = 9;
    } else {
        m_isMediumRushSection = false;
        m_field->m_aimZ = 10 + m_random.nextSignedFloat(0.3);
    }

    m_bossSection = false;
    if (m_section == 2) {
        m_sectionIntervalCnt = 2 * 60;
    } else if (m_section == 3) {
        m_sectionIntervalCnt = 4 * 60;
    } else {
        m_sectionIntervalCnt = 1 * 60;
    }
    m_sectionCnt = m_sectionIntervalCnt + 10 * 60;

    int sp = m_section * 3 / 7 + 1;
    int ep = 3 + m_section * 3 / 10;
    int ap = sp + m_random.nextInt(ep - sp + 1);

    if (m_section == 0) {
        ap = 0;
    } else if (m_isMediumRushSection) {
        ap = MEDIUM_RUSH_SECTION_PATTERN;
    }

    for (int i = 0; i < m_fleetPatterns[m_gameManager->m_mode][ap][0]; ++i, ++m_fleetTotal) {
        EnemyFleet *appearance = &m_fleets[m_fleetTotal];
        setFleet(appearance, SMALL);
    }
    for (int i = 0; i < m_fleetPatterns[m_gameManager->m_mode][ap][1]; ++i, ++m_fleetTotal) {
        EnemyFleet *appearance = &m_fleets[m_fleetTotal];
        setFleet(appearance, MEDIUM);
    }
    for (int i = 0; i < m_fleetPatterns[m_gameManager->m_mode][ap][2]; ++i, ++m_fleetTotal) {
        EnemyFleet *appearance = &m_fleets[m_fleetTotal];
        setFleet(appearance, LARGE);
    }
}

void StageManager::createStage() {
    createEnemyData();

    m_mediumRushSection = 2 + m_random.nextInt(6);

    if (m_mediumRushSection <= 4) {
        m_mediumRushSection++;
    }

    m_field->setType(m_stageType % Field::TYPE_NUMBER);
    ++m_stageType;
}

void StageManager::createEnemyData() {
    for (int i = 0; i < m_smallType.size(); ++i) {
        m_smallType[i]->setSmallEnemyType(m_rank, m_gameManager->m_mode);
    }
    for (int i = 0; i < m_mediumType.size(); ++i) {
        m_mediumType[i]->setMediumEnemyType(m_rank, m_gameManager->m_mode);
    }
    for (int i = 0; i < m_largeType.size(); ++i) {
        m_largeType[i]->setLargeEnemyType(m_rank, m_gameManager->m_mode);
    }
    m_mediumBossType->setMediumBossEnemyType(m_rank, m_gameManager->m_mode);
    m_largeBossType->setMediumBossEnemyType(m_rank, m_gameManager->m_mode);
}

void StageManager::setFleetPattern(EnemyFleet *appearance) {
    switch (m_random.nextInt(5)) {
    case 0:
        appearance->m_pattern = ONE_SIDE;
        break;
    case 1:
    case 2:
        appearance->m_pattern = ALTERNATE;
        break;
    case 3:
    case 4:
        appearance->m_pattern = BOTH_SIDES;
        break;
    }

    switch (m_random.nextInt(3)) {
    case 0:
        appearance->m_sequence = RANDOM;
        break;
    case 1:
    case 2:
        appearance->m_sequence = FIXED;
        break;
    }
}

void StageManager::setFleet(EnemyFleet *fleet, int type) {
    switch (type) {
    case SMALL:
        setSmallFleet(fleet);
        break;
    case MEDIUM:
        setMediumFleet(fleet);
        break;
    case LARGE:
        setLargeFleet(fleet);
        break;
    }

    fleet->m_cnt = 0;
    fleet->m_remaining = fleet->m_total;
    fleet->m_side = m_random.nextInt(2) * 2 - 1;
    fleet->m_position = m_random.nextFloat(1);
}

void StageManager::setSmallFleet(EnemyFleet *fleet) {
    fleet->m_type = m_smallType[m_random.nextInt(m_smallType.size())];

    int mt;
    if (m_random.nextFloat(1) > 0.2) {
        fleet->m_point = TOP;
        mt = BarrageManager::SMALL_MOVE;
    } else {
        fleet->m_point = SIDE;
        mt = BarrageManager::SMALL_SIDE_MOVE;
    }

    fleet->m_moveParser = m_barrageManager->m_parser[mt][m_random.nextInt(m_barrageManager->m_parserSizes[mt])];
    setFleetPattern(fleet);

    if (fleet->m_pattern == ONE_SIDE) {
        fleet->m_pattern = ALTERNATE;
    }

    switch (m_random.nextInt(4)) {
    case 0:
        fleet->m_total = 7 + m_random.nextInt(5);
        fleet->m_groupInterval = 72 + m_random.nextInt(15);
        fleet->m_interval = 15 + m_random.nextInt(5);
        break;
    case 1:
        fleet->m_total = 5 + m_random.nextInt(3);
        fleet->m_groupInterval = 56 + m_random.nextInt(10);
        fleet->m_interval = 20 + m_random.nextInt(5);
        break;
    case 2:
    case 3:
        fleet->m_total = 2 + m_random.nextInt(2);
        fleet->m_groupInterval = 45 + m_random.nextInt(20);
        fleet->m_interval = 25 + m_random.nextInt(5);
        break;
    }
    //UE_LOG(LogTemp, Warning, TEXT(" !!!! setSmallFleet : [tot: %f, grp: %f, int: %f] "), fleet->m_total, fleet->m_groupInterval, fleet->m_interval);
}

void StageManager::setMediumFleet(EnemyFleet *appearance) {
    appearance->m_type = m_mediumType[m_random.nextInt(m_mediumType.size())];

    int mt;
    /*
    @NOTE: appearance from behind is disabled for medium enemies
    if (m_random.nextFloat(1) > 0.1) {
        appearance->m_point = TOP;
        mt = BarrageManager::MEDIUM_MOVE;
    } else {
        appearance->m_point = BACK;
        mt = BarrageManager::MEDIUM_BACK_MOVE;
    }
    */
    appearance->m_point = TOP;
    mt = BarrageManager::MEDIUM_MOVE;

    appearance->m_moveParser = m_barrageManager->m_parser[mt][m_random.nextInt(m_barrageManager->m_parserSizes[mt])];
    setFleetPattern(appearance);

    switch (m_random.nextInt(3)) {
    case 0:
        appearance->m_total = 4;
        appearance->m_groupInterval = 240 + m_random.nextInt(150);
        appearance->m_interval = 80 + m_random.nextInt(30);
        break;
    case 1:
        appearance->m_total = 2;
        appearance->m_groupInterval = 180 + m_random.nextInt(60);
        appearance->m_interval = 180 + m_random.nextInt(20);
        break;
    case 2:
        appearance->m_total = 1;
        appearance->m_groupInterval = 150 + m_random.nextInt(50);
        appearance->m_interval = 100;
        break;
    }
}

void StageManager::setLargeFleet(EnemyFleet *appearance) {
    appearance->m_type = m_largeType[m_random.nextInt(m_largeType.size())];

    int mt;
    appearance->m_point = TOP;
    mt = BarrageManager::LARGE_MOVE;

    appearance->m_moveParser = m_barrageManager->m_parser[mt][m_random.nextInt(m_barrageManager->m_parserSizes[mt])];
    setFleetPattern(appearance);

    switch (m_random.nextInt(3)) {
    case 0:
        appearance->m_total = 3;
        appearance->m_groupInterval = 400 + m_random.nextInt(100);
        appearance->m_interval = 240 + m_random.nextInt(40);
        break;
    case 1:
        appearance->m_total = 2;
        appearance->m_groupInterval = 400 + m_random.nextInt(60);
        appearance->m_interval = 300 + m_random.nextInt(20);
        break;
    case 2:
        appearance->m_total = 1;
        appearance->m_groupInterval = 270 + m_random.nextInt(50);
        appearance->m_interval = 200;
        break;
    }
}

void StageManager::gotoNextSection() {
    ++m_section;
    ++m_parsec;

    if (m_gameManager->m_state == AGameManager::TITLE &&
        m_section >= 4) {
        m_section = 0;
        m_parsec -= 4;
    }

    if (m_section >= 10) {
        m_section = 0;
        m_rank += m_rankInc;
        createStage();
    }

    createSectionData();
}