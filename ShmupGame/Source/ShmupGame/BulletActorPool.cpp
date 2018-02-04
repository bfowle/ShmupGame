#include "BulletActorPool.h"

#include "ActorInitializer.h"
#include "ActorPool.h"
#include "Bullet.h"
#include "BulletActor.h"
#include "ShmupBullet.h"

#include "bulletml/bulletml.h"

using namespace std;

BulletActorPool::BulletActorPool(int n, shared_ptr<ActorInitializer> initializer) :
    m_cnt(0),
    ActorPool::ActorPool(n, unique_ptr<BulletActor>(new BulletActor()).get(), initializer) {
    Bullet::setBulletManager(this);
    BulletActor::init();
}

// called via bulletml createSimpleBullet
void BulletActorPool::addBullet(float direction, float speed) {
    shared_ptr<BulletActor> bullet = static_pointer_cast<BulletActor>(getInstance());
    if (!bullet) {
        return;
    }

    ShmupBullet *rb = static_cast<ShmupBullet *>(Bullet::m_now);
    if (rb->m_isMorph) {
        // @FIXME: temp fix
        if (rb->m_morphIdx < 0) {
            return;
        }
        BulletMLRunner *runner = BulletMLRunner_new_parser(rb->m_morphParser[rb->m_morphIdx]);
        BulletActorPool::registerFunctions(runner);
        bullet->set(runner, Bullet::m_now->m_position.X, Bullet::m_now->m_position.Y,
            direction, speed, Bullet::m_now->m_rank,
            rb->m_speedRank, rb->m_xReverse,
            rb->m_morphParser, rb->m_morphSize,
            rb->m_morphIdx + 1, rb->m_morphCnt - 1);
    } else {
        bullet->set(Bullet::m_now->m_position.X, Bullet::m_now->m_position.Y,
            direction, speed, Bullet::m_now->m_rank,
            rb->m_speedRank, rb->m_xReverse);
    }
    bullet->spawnBulletActor();
}

// called via bulletml createBullet
void BulletActorPool::addBullet(BulletMLState *state, float direction, float speed) {
    shared_ptr<BulletActor> bullet = static_pointer_cast<BulletActor>(getInstance());
    if (!bullet) {
        return;
    }

    BulletMLRunner *runner = BulletMLRunner_new_state(state);
    registerFunctions(runner);

    ShmupBullet *rb = static_cast<ShmupBullet *>(Bullet::m_now);
    if (rb->m_isMorph) {
        bullet->set(runner, Bullet::m_now->m_position.X, Bullet::m_now->m_position.Y,
            direction, speed, Bullet::m_now->m_rank,
            rb->m_speedRank, rb->m_xReverse,
            rb->m_morphParser, rb->m_morphSize,
            rb->m_morphIdx, rb->m_morphCnt);
    } else {
        bullet->set(runner, Bullet::m_now->m_position.X, Bullet::m_now->m_position.Y,
            direction, speed, Bullet::m_now->m_rank,
            rb->m_speedRank, rb->m_xReverse);
    }
    bullet->spawnBulletActor();
}

// called via enemy move bullet
shared_ptr<BulletActor> BulletActorPool::addBullet(BulletMLRunner *runner, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse) {
    shared_ptr<BulletActor> bullet = static_pointer_cast<BulletActor>(getInstance());
    if (!bullet) {
        shared_ptr<BulletActor> null;
        return null;
    }

    bullet->set(runner, x, y, direction, speed, rank, speedRank, xReverse);
    bullet->setInvisible();
    return bullet;
}

// called via enemy top bullet
shared_ptr<BulletActor> BulletActorPool::addBullet(BulletMLParser *parser, BulletMLRunner *runner, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse) {
    shared_ptr<BulletActor> bullet = addBullet(runner, x, y, direction, speed, rank, speedRank, xReverse);
    if (!bullet) {
        shared_ptr<BulletActor> null;
        return null;
    }

    bullet->setTop(parser);
    return bullet;
}

// called via enemy top bullet battery
shared_ptr<BulletActor> BulletActorPool::addBullet(BulletMLParser *parser, BulletMLRunner *runner, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse, array<BulletMLParser *, MorphBullet::MORPH_MAX> morph, int morphSize, int morphCnt) {
    shared_ptr<BulletActor> bullet = static_pointer_cast<BulletActor>(getInstance());
    if (!bullet) {
        shared_ptr<BulletActor> null;
        return null;
    }

    bullet->set(runner, x, y, direction, speed, rank, speedRank, xReverse, morph, morphSize, 0, morphCnt);
    bullet->setTop(parser);
    return bullet;
}

void BulletActorPool::tick() {
    ActorPool::tick();
    ++m_cnt;
}

int BulletActorPool::getTurn() {
    return m_cnt;
}

void BulletActorPool::kill(Bullet *bullet) {
    //assert(static_pointer_cast<BulletActor>(actor[bullet->id])->bullet->id == bullet->id);
    static_pointer_cast<BulletActor>(m_actor[bullet->m_id])->remove();
}

void BulletActorPool::clear() {
    for (int i = 0; i < m_actor.size(); ++i) {
        if (m_actor[i]->m_exists) {
            static_pointer_cast<BulletActor>(m_actor[i])->remove();
        }
    }
}

void BulletActorPool::registerFunctions(BulletMLRunner *runner) {
    BulletMLRunner_set_getBulletDirection(runner, &getBulletDirection_);
    BulletMLRunner_set_getAimDirection(runner, &getAimDirectionWithXRev_);
    BulletMLRunner_set_getBulletSpeed(runner, &getBulletSpeed_);
    BulletMLRunner_set_getDefaultSpeed(runner, &getDefaultSpeed_);
    BulletMLRunner_set_getRank(runner, &getRank_);
    BulletMLRunner_set_createSimpleBullet(runner, &createSimpleBullet_);
    BulletMLRunner_set_createBullet(runner, &createBullet_);
    BulletMLRunner_set_getTurn(runner, &getTurn_);
    BulletMLRunner_set_doVanish(runner, &doVanish_);

    BulletMLRunner_set_doChangeDirection(runner, &doChangeDirection_);
    BulletMLRunner_set_doChangeSpeed(runner, &doChangeSpeed_);
    BulletMLRunner_set_doAccelX(runner, &doAccelX_);
    BulletMLRunner_set_doAccelY(runner, &doAccelY_);
    BulletMLRunner_set_getBulletSpeedX(runner, &getBulletSpeedX_);
    BulletMLRunner_set_getBulletSpeedY(runner, &getBulletSpeedY_);
    BulletMLRunner_set_getRand(runner, &getRand_);
}

#ifdef __cplusplus
extern "C" {
#endif
    double getAimDirectionWithXRev_(BulletMLRunner *runner) {
        float xrev = static_cast<ShmupBullet *>(Bullet::m_now)->m_xReverse;
        return rtod(atan2(Bullet::m_target.X - Bullet::m_now->m_position.X,
            Bullet::m_target.Y - Bullet::m_now->m_position.Y) * xrev);
    }
#ifdef __cplusplus
}
#endif