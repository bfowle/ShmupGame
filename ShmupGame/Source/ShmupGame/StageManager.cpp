#include "StageManager.h"

#include "GameManager.h"
#include "Enemy.h"
#include "Field.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"

using namespace std;

const int StageManager::m_appearancePattern[][7][3] = {
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

void StageManager::init(shared_ptr<Field> field, shared_ptr<BarrageManager> barrageManager, AGameManager *gameManager) {
    m_field = field;
    m_barrageManager = barrageManager;
    m_gameManager = gameManager;

    m_random = Random();
    m_apos = FVector2D();

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
    for (int i = 0; i < m_apNum; ++i) {
        EnemyAppearance *appearance = &m_appearance[i];
        appearance->m_cnt--;

        if (appearance->m_cnt > 0) {
            // add an extra enemy
            if (!m_middleRushSection) {
                if (appearance->m_type->m_type == EnemyType::SMALL &&
                    !EnemyType::m_exists[appearance->m_type->m_id]) {
                    appearance->m_cnt = 0;
                    EnemyType::m_exists[appearance->m_type->m_id] = true;
                }
            } else {
                if (appearance->m_type->m_type == EnemyType::MEDIUM &&
                    !EnemyType::m_exists[appearance->m_type->m_id]) {
                    appearance->m_cnt = 0;
                    EnemyType::m_exists[appearance->m_type->m_id] = true;
                }
            }
            continue;
        }

        float p = 0;
        switch (appearance->m_sequence) {
        case RANDOM:
            p = m_random.nextFloat(1);
            break;
        case FIXED:
            p = appearance->m_position;
            break;
        }

        float d = 0;
        switch (appearance->m_point) {
        case TOP:
            switch (appearance->m_pattern) {
            case BOTH_SIDES:
                m_apos.X = (p - 0.5) * m_field->m_size.X * 1.8;
                break;
            default:
                m_apos.X = (p * 0.6 + 0.2) * m_field->m_size.X * appearance->m_side;
                break;
            }
            m_apos.Y = m_field->m_size.Y - Enemy::FIELD_SPACE;
            d = M_PI;
            break;

        case BACK:
            switch (appearance->m_pattern) {
            case BOTH_SIDES:
                m_apos.X = (p - 0.5) * m_field->m_size.X * 1.8;
                break;
            default:
                m_apos.X = (p * 0.6 + 0.2) * m_field->m_size.X * appearance->m_side;
                break;
            }
            m_apos.Y = -m_field->m_size.Y + Enemy::FIELD_SPACE;
            d = 0;
            break;

        case SIDE:
            switch (appearance->m_pattern) {
            case BOTH_SIDES:
                m_apos.X = (m_field->m_size.X - Enemy::FIELD_SPACE) * (m_random.nextInt(2) * 2 - 1);
                break;
            default:
                m_apos.X = (m_field->m_size.X - Enemy::FIELD_SPACE) * appearance->m_side;
                break;
            }
            m_apos.Y = (p * 0.4 + 0.4) * m_field->m_size.Y;
            if (m_apos.X < 0) {
                d = M_PI / 2;
            } else {
                d = M_PI / 2 * 3;
            }
            break;
        }
        m_apos.X *= 0.88;

        m_gameManager->addEnemy(m_apos, d, appearance->m_type, appearance->m_moveParser);

        appearance->m_left--;
        if (appearance->m_left <= 0) {
            appearance->m_cnt = appearance->m_groupInterval;
            appearance->m_left = appearance->m_num;
            if (appearance->m_pattern != ONE_SIDE) {
                appearance->m_side *= -1;
            }
            appearance->m_position = m_random.nextFloat(1);
        } else {
            appearance->m_cnt = appearance->m_interval;
        }
    }

    if (!m_bossSection ||
        (!EnemyType::m_exists[m_mediumBossType->m_id] &&
         !EnemyType::m_exists[m_largeBossType->m_id])) {
        --m_sectionCnt;
    }

    if (m_sectionCnt < m_sectionIntervalCnt) {
        if (m_section == 9 &&
            m_sectionCnt == m_sectionIntervalCnt - 1) {
            // do stuff
        }
        m_apNum = 0;

        if (m_sectionCnt <= 0) {
            gotoNextSection();
        }
    }
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

void StageManager::setAppearancePattern(EnemyAppearance *appearance) {
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

void StageManager::setSmallAppearance(EnemyAppearance *appearance) {
    appearance->m_type = m_smallType[m_random.nextInt(m_smallType.size())];

    int mt;
    if (m_random.nextFloat(1) > 0.2) {
        appearance->m_point = TOP;
        mt = BarrageManager::SMALL_MOVE;
    } else {
        appearance->m_point = SIDE;
        mt = BarrageManager::SMALL_SIDE_MOVE;
    }

    appearance->m_moveParser = m_barrageManager->m_parser[mt][m_random.nextInt(m_barrageManager->m_parserNum[mt])];
    setAppearancePattern(appearance);

    if (appearance->m_pattern == ONE_SIDE) {
        appearance->m_pattern = ALTERNATE;
    }

    switch (m_random.nextInt(4)) {
    case 0:
        appearance->m_num = 7 + m_random.nextInt(5);
        appearance->m_groupInterval = 72 + m_random.nextInt(15);
        appearance->m_interval = 15 + m_random.nextInt(5);
        break;
    case 1:
        appearance->m_num = 5 + m_random.nextInt(3);
        appearance->m_groupInterval = 56 + m_random.nextInt(10);
        appearance->m_interval = 20 + m_random.nextInt(5);
        break;
    case 2:
    case 3:
        appearance->m_num = 2 + m_random.nextInt(2);
        appearance->m_groupInterval = 45 + m_random.nextInt(20);
        appearance->m_interval = 25 + m_random.nextInt(5);
        break;
    }
}

void StageManager::setMediumAppearance(EnemyAppearance *appearance) {
    appearance->m_type = m_mediumType[m_random.nextInt(m_mediumType.size())];

    int mt;
    /*
    @NOTE: appearance from behind is disabled for medium enemies

    if (m_random.nextFloat(1) > 0.1) {
        appearance->m_point = TOP;
        mt = BarrageManager::MIDDLE_MOVE;
    } else {
        appearance->m_point = BACK;
        mt = BarrageManager::MIDDLE_BACK_MOVE;
    }
    */
    appearance->m_point = TOP;
    mt = BarrageManager::MEDIUM_MOVE;

    appearance->m_moveParser = m_barrageManager->m_parser[mt][m_random.nextInt(m_barrageManager->m_parserNum[mt])];
    setAppearancePattern(appearance);

    switch (m_random.nextInt(3)) {
    case 0:
        appearance->m_num = 4;
        appearance->m_groupInterval = 240 + m_random.nextInt(150);
        appearance->m_interval = 80 + m_random.nextInt(30);
        break;
    case 1:
        appearance->m_num = 2;
        appearance->m_groupInterval = 180 + m_random.nextInt(60);
        appearance->m_interval = 180 + m_random.nextInt(20);
        break;
    case 2:
        appearance->m_num = 1;
        appearance->m_groupInterval = 150 + m_random.nextInt(50);
        appearance->m_interval = 100;
        break;
    }
}

void StageManager::setLargeAppearance(EnemyAppearance *appearance) {
    appearance->m_type = m_largeType[m_random.nextInt(m_largeType.size())];

    int mt;
    appearance->m_point = TOP;
    mt = BarrageManager::LARGE_MOVE;

    appearance->m_moveParser = m_barrageManager->m_parser[mt][m_random.nextInt(m_barrageManager->m_parserNum[mt])];
    setAppearancePattern(appearance);

    switch (m_random.nextInt(3)) {
    case 0:
        appearance->m_num = 3;
        appearance->m_groupInterval = 400 + m_random.nextInt(100);
        appearance->m_interval = 240 + m_random.nextInt(40);
        break;
    case 1:
        appearance->m_num = 2;
        appearance->m_groupInterval = 400 + m_random.nextInt(60);
        appearance->m_interval = 300 + m_random.nextInt(20);
        break;
    case 2:
        appearance->m_num = 1;
        appearance->m_groupInterval = 270 + m_random.nextInt(50);
        appearance->m_interval = 200;
        break;
    }
}

void StageManager::setAppearance(EnemyAppearance *appearance, int type) {
    switch (type) {
    case SMALL:
        setSmallAppearance(appearance);
        break;
    case MEDIUM:
        setMediumAppearance(appearance);
        break;
    case LARGE:
        setLargeAppearance(appearance);
        break;
    }

    appearance->m_cnt = 0;
    appearance->m_left = appearance->m_num;
    appearance->m_side = m_random.nextInt(2) * 2 - 1;
    appearance->m_position = m_random.nextFloat(1);
}

void StageManager::createSectionData() {
    m_apNum = 0;
    if (m_rank <= 0) {
        return;
    }

    m_field->m_aimSpeed = 0.1 + m_section * 0.02;

    if (m_section == 4) {
        // set the middle boss
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
    } else if (m_section == m_middleRushSectionNum) {
        // no small enemies in this section
        m_middleRushSection = true;
        m_field->m_aimZ = 9;
    } else {
        m_middleRushSection = false;
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
    } else if (m_middleRushSection) {
        ap = MIDDLE_RUSH_SECTION_PATTERN;
    }

    for (int i = 0; i < m_appearancePattern[m_gameManager->m_mode][ap][0]; ++i, ++m_apNum) {
        EnemyAppearance *appearance = &m_appearance[m_apNum];
        setAppearance(appearance, SMALL);
    }
    for (int i = 0; i < m_appearancePattern[m_gameManager->m_mode][ap][1]; ++i, ++m_apNum) {
        EnemyAppearance *appearance = &m_appearance[m_apNum];
        setAppearance(appearance, MEDIUM);
    }
    for (int i = 0; i < m_appearancePattern[m_gameManager->m_mode][ap][2]; ++i, ++m_apNum) {
        EnemyAppearance *appearance = &m_appearance[m_apNum];
        setAppearance(appearance, LARGE);
    }
}

void StageManager::createStage() {
    createEnemyData();

    m_middleRushSectionNum = 2 + m_random.nextInt(6);

    if (m_middleRushSectionNum <= 4) {
        m_middleRushSectionNum++;
    }

    m_field->setType(m_stageType % Field::TYPE_NUMBER);
    ++m_stageType;
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