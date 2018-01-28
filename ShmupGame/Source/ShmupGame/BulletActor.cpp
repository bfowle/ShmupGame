#include "BulletActor.h"

#include "BulletActorInitializer.h"
#include "BulletActorPool.h"
#include "Field.h"
#include "Ship.h"

#include "bulletml/bulletml.h"
#include "bulletml/bulletmlparser.h"

#include "GameFramework/Actor.h"

using namespace std;

float BulletActor::m_totalBulletsSpeed;
int BulletActor::m_nextId = 0;
const float BulletActor::FIELD_SPACE = 0.5;
const float BulletActor::SHAPE_POINT_SIZE = 0.1;
/*
const float BulletActor::SHAPE_BASE_COLOR_R = 1;
const float BulletActor::SHAPE_BASE_COLOR_G = 0.9;
const float BulletActor::SHAPE_BASE_COLOR_B = 0.7;
const float BulletActor::bulletColor[4][3] = {
    {1, 0, 0},
    {0.2, 1, 0.4},
    {0.3, 0.3, 1},
    {1, 1, 0}
};
*/

shared_ptr<Actor> BulletActor::newActor() {
    return shared_ptr<Actor>(new BulletActor());
}

void BulletActor::init() {
    m_nextId = 0;
}

void BulletActor::init(shared_ptr<ActorInitializer> initializer) {
    shared_ptr<BulletActorInitializer> bullet = static_pointer_cast<BulletActorInitializer>(initializer);
    m_field = bullet->m_field;
    m_ship = bullet->m_ship;

    m_bullet.reset(new ShmupBullet(m_nextId));
    m_previousPosition = FVector2D();
    ++m_nextId;
}

void BulletActor::set(BulletMLRunner *runner, float x, float y, float direction, float speed, float rank, float speedRank, int shape, int color, int size, float xReverse) {
    m_bullet->set(runner, x, y, direction, speed, rank);
    m_bullet->m_isMorph = false;
    m_isSimple = false;
    start(speedRank, shape, color, size, xReverse);
}

void BulletActor::set(BulletMLRunner *runner, float x, float y, float direction, float speed, float rank, float speedRank, int shape, int color, int size, float xReverse, array<BulletMLParser *, MorphBullet::MORPH_MAX> morph, int morphNum, int morphIdx, int morphCnt) {
    m_bullet->set(runner, x, y, direction, speed, rank);
    m_bullet->setMorph(morph, morphNum, morphIdx, morphCnt);
    m_isSimple = false;
    start(speedRank, shape, color, size, xReverse);
}

void BulletActor::set(float x, float y, float direction, float speed, float rank, float speedRank, int shape, int color, int size, float xReverse) {
    m_bullet->set(x, y, direction, speed, rank);
    m_bullet->m_isMorph = false;
    m_isSimple = true;
    start(speedRank, shape, color, size, xReverse);
}

void BulletActor::start(float speedRank, int shape, int color, float size, float xReverse) {
    m_exists = true;
    m_isTop = false;
    m_isVisible = true;
    m_previousPosition.X = m_bullet->m_position.X;
    m_previousPosition.Y = m_bullet->m_position.Y;
    m_bullet->setParam(speedRank, shape, color, size, xReverse);
    m_cnt = 0;
    m_rtCnt = 0;
    m_shouldBeRemoved = false;
    m_backToRetro = false;
}

void BulletActor::setInvisible() {
    m_isVisible = false;
}

void BulletActor::setTop(BulletMLParser *parser) {
    m_parser = parser;
    m_isTop = true;
    setInvisible();
}

void BulletActor::rewind() {
    m_bullet->remove();

    BulletMLRunner *runner = BulletMLRunner_new_parser(m_parser);
    BulletActorPool::registerFunctions(runner);
    m_bullet->setRunner(runner);
    m_bullet->resetMorph();
}

void BulletActor::remove() {
    m_shouldBeRemoved = true;
}

void BulletActor::removeForced() {
    if (!m_isSimple) {
        m_bullet->remove();
    }
    m_exists = false;
}

void BulletActor::toRetro() {
    if (!m_isVisible ||
        m_backToRetro) {
        return;
    }
    m_backToRetro = true;

    if (m_rtCnt >= RETRO_COUNT) {
        m_rtCnt = RETRO_COUNT - 0.1;
    }
}

void BulletActor::tick() {
    m_previousPosition.X = m_bullet->m_position.X;
    m_previousPosition.Y = m_bullet->m_position.Y;

    if (!m_isSimple) {
        m_bullet->tick();
        if (m_isTop &&
            m_bullet->isEnd()) {
            rewind();
        }
    }

    if (m_shouldBeRemoved) {
        removeForced();
        return;
    }

    float sr = 0;
    if (m_rtCnt < RETRO_COUNT) {
        sr = m_bullet->m_speedRank * (0.3 + (m_rtCnt / RETRO_COUNT) * 0.7);
        if (m_backToRetro) {
            m_rtCnt -= sr;
            if (m_rtCnt <= 0) {
                removeForced();
                return;
            }
        } else {
            m_rtCnt += sr;
        }
        if (m_ship->m_cnt < -m_ship->INVINCIBLE_COUNT / 2 &&
            m_isVisible && m_rtCnt >= RETRO_COUNT) {
            removeForced();
            return;
        }
    } else {
        sr = m_bullet->m_speedRank;
        if (m_cnt > BULLET_DISAPPEAR_COUNT) {
            toRetro();
        }
    }

    m_bullet->m_position.X += (sin(m_bullet->m_direction) * m_bullet->m_speed + m_bullet->m_acceleration.X) * sr * m_bullet->m_xReverse;
    m_bullet->m_position.Y -= (cos(m_bullet->m_direction) * m_bullet->m_speed - m_bullet->m_acceleration.Y) * sr;

    if (m_actor != nullptr) {
        //UE_LOG(LogTemp, Warning, TEXT("  -- Bullet::tick [%s] (%s)"), *m_actor->GetName(), *m_bullet->m_position.ToString());

        m_actor->SetActorLocation(FVector(m_bullet->m_position.X, 100.0, m_bullet->m_position.Y));
    }

    if (m_isVisible) {
        m_totalBulletsSpeed += m_bullet->m_speed * sr;

        if (m_rtCnt > RETRO_COUNT) {
            //checkShipHit();
        }

        if (m_field->checkHit(m_bullet->m_position, FIELD_SPACE)) {
            removeForced();
        }
    }

    m_cnt++;
}

void BulletActor::resetTotalBulletsSpeed() {
    m_totalBulletsSpeed = 0;
}