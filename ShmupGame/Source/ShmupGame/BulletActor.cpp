#include "BulletActor.h"

#include "BulletActorInitializer.h"

using namespace std;

float BulletActor::m_totalBulletsSpeed;
int BulletActor::m_nextId = 0;

shared_ptr<Actor> BulletActor::newActor() {
    return shared_ptr<Actor>(new BulletActor());
}

void BulletActor::init() {
    m_nextId = 0;
}

void BulletActor::init(shared_ptr<ActorInitializer> initializer) {
    shared_ptr<BulletActorInitializer> bullet = static_pointer_cast<BulletActorInitializer>(initializer);
    //m_field = bullet->field;
    //m_ship = bullet->ship;
    m_bullet.reset(new ShmupBullet(m_nextId));
    m_position = FVector2D();
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
    m_position.X = m_bullet->m_position.X;
    m_position.Y = m_bullet->m_position.Y;
    m_bullet->setParam(speedRank, shape, color, size, xReverse);
    m_cnt = 0;
    m_shouldBeRemoved = false;
}

void BulletActor::setInvisible() {
    m_isVisible = false;
}

void BulletActor::setTop(BulletMLParser *parser) {
    m_parser = parser;
    m_isTop = true;
    setInvisible();
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

void BulletActor::tick() {
    m_position.X = m_bullet->m_position.X;
    m_position.Y = m_bullet->m_position.Y;

    if (!m_isSimple) {
        m_bullet->tick();
        //if (m_isTop && m_bullet->isEnd()) {
        //    rewind();
        //}
    }

    if (m_shouldBeRemoved) {
        removeForced();
        return;
    }

    /*
    float sr;
    if (rtCnt < RETRO_CNT) {
        sr = bullet->speedRank * (0.3 + (rtCnt / RETRO_CNT) * 0.7);
        if (backToRetro) {
            rtCnt -= sr;
            if (rtCnt <= 0) {
                removeForced();
                return;
            }
        } else {
            rtCnt += sr;
        }
        if (ship->cnt < -ship->INVINCIBLE_CNT / 2 && isVisible && rtCnt >= RETRO_CNT) {
            removeForced();
            return;
        }
    } else {
        sr = bullet->speedRank;
        if (cnt > BULLET_DISAPPEAR_CNT)
            toRetro();
    }
    bullet->pos.x +=
        (sin(bullet->deg) * bullet->speed + bullet->acc.x) * sr * bullet->xReverse;
    bullet->pos.y +=
        (cos(bullet->deg) * bullet->speed - bullet->acc.y) * sr;
    if (isVisible) {
        totalBulletsSpeed += bullet->speed * sr;
        if (rtCnt > RETRO_CNT)
            checkShipHit();
        if (field->checkHit(bullet->pos, FIELD_SPACE))
            removeForced();
    }
    */

    m_cnt++;
}

void BulletActor::resetTotalBulletsSpeed() {
    m_totalBulletsSpeed = 0;
}