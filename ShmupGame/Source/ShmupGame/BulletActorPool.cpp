#include "BulletActorPool.h"

#include "Actor.h"
#include "ActorPool.h"
#include "Bullet.h"
#include "BulletActor.h"
#include "ShmupBullet.h"

#include "bulletml/bulletml.h"

using namespace std;

BulletActorPool::BulletActorPool(int size, shared_ptr<ActorInitializer> initializer) :
    m_cnt(0),
    ActorPool::ActorPool(size, unique_ptr<BulletActor>(new BulletActor()).get(), initializer) {
    Bullet::setBulletManager(this);
    BulletActor::init();
}

// called via bulletml createSimpleBullet
void BulletActorPool::addBullet(float x, float y, float direction, float speed, BulletActor *parent) {
    shared_ptr<BulletActor> bullet = static_pointer_cast<BulletActor>(getInstance());
    if (!bullet) {
        return;
    }

    ShmupBullet *rb = static_cast<ShmupBullet *>(parent->m_bullet.get());
    //UE_LOG(LogTemp, Warning, TEXT(" BulletActorPool::addBullet[createSimpleBullet] -> %f, %f "), x, y);
    if (rb->m_isMorph) {
        // @FIXME: temp fix
        if (rb->m_morphIdx < 0) {
            return;
        }

        BulletCommand *bc = new BulletCommand(rb->m_morphParser[rb->m_morphIdx], bullet->m_bullet.get());
        bullet->set(bc, x, y, direction, speed, bullet->m_bullet->m_rank,
            rb->m_speedRank, rb->m_xReverse,
            rb->m_morphParser, rb->m_morphSize,
            rb->m_morphIdx + 1, rb->m_morphCnt - 1);
    } else {
        bullet->set(x, y, direction, speed, bullet->m_bullet->m_rank,
            rb->m_speedRank, rb->m_xReverse);
    }
    bullet->spawnBulletActor();
}

// called via bulletml createBullet
void BulletActorPool::addBullet(BulletMLState *state, float x, float y, float direction, float speed, BulletActor *parent) {
    shared_ptr<BulletActor> bullet = static_pointer_cast<BulletActor>(getInstance());
    if (!bullet) {
        return;
    }

    ShmupBullet *rb = static_cast<ShmupBullet *>(parent->m_bullet.get());
    //UE_LOG(LogTemp, Warning, TEXT(" BulletActorPool::addBullet[createBullet] -> %f, %f "), x, y);
    BulletCommand *bc = new BulletCommand(state, bullet->m_bullet.get());
    if (rb->m_isMorph) {
        bullet->set(bc, x, y, direction, speed, bullet->m_bullet->m_rank,
            rb->m_speedRank, rb->m_xReverse,
            rb->m_morphParser, rb->m_morphSize,
            rb->m_morphIdx, rb->m_morphCnt);
    } else {
        bullet->set(bc, x, y, direction, speed, bullet->m_bullet->m_rank,
            rb->m_speedRank, rb->m_xReverse);
    }

    const vector<BulletMLNode *> &actions = state->getNode();
    vector<BulletMLNode *>::const_iterator iter;
    for (iter = actions.cbegin(); iter != actions.cend(); iter++) {
        const BulletMLNode *action = *iter;
        if (action->findNode(BulletMLNode::fire) ||
            action->findNode(BulletMLNode::fireRef) ||
            action->findNode(BulletMLNode::actionRef)) {
            break;
        }
    }

    if (iter != actions.end()) {
        //UE_LOG(LogTemp, Warning, TEXT(" -> spawn bullet actor :: TRUE "));
    } else {
        //UE_LOG(LogTemp, Warning, TEXT(" -> spawn bullet actor :: FALSE "));
    }

    bullet->spawnBulletActor();
}

// called via enemy move bullet
shared_ptr<BulletActor> BulletActorPool::addBullet(BulletMLParser *parser, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse) {
    shared_ptr<BulletActor> bullet = static_pointer_cast<BulletActor>(getInstance());
    if (!bullet) {
        shared_ptr<BulletActor> null;
        return null;
    }

    //UE_LOG(LogTemp, Warning, TEXT(" BulletActorPool::addBullet[moveBullet] -> %f, %f "), x, y);
    BulletCommand *bc = new BulletCommand(parser, bullet->m_bullet.get());
    bullet->set(bc, x, y, direction, speed, rank, speedRank, xReverse);
    bullet->setInvisible();
    return bullet;
}

// called via enemy top bullet
shared_ptr<BulletActor> BulletActorPool::addBullet(BulletMLParser *parser, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse, array<BulletMLParser *, MorphBullet::MORPH_MAX> morph, int morphSize, int morphCnt) {
    shared_ptr<BulletActor> bullet = static_pointer_cast<BulletActor>(getInstance());
    if (!bullet) {
        shared_ptr<BulletActor> null;
        return null;
    }

    //UE_LOG(LogTemp, Warning, TEXT(" BulletActorPool::addBullet[topBullet] -> %f, %f "), x, y);
    BulletCommand *bc = new BulletCommand(parser, bullet->m_bullet.get());
    bullet->set(bc, x, y, direction, speed, rank, speedRank, xReverse,
        morph, morphSize, 0, morphCnt);
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
    static_pointer_cast<BulletActor>(m_pool[bullet->m_id])->remove();
}

void BulletActorPool::clear() {
    for (int i = 0; i < m_pool.size(); ++i) {
        if (m_pool[i]->m_isAlive) {
            static_pointer_cast<BulletActor>(m_pool[i])->remove();
        }
    }
}