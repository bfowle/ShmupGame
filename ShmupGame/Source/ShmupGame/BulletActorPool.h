#ifndef BULLETACTORPOOL_H
#define BULLETACTORPOOL_H

#include "ActorPool.h"
#include "BulletManager.h"
#include "MorphBullet.h"

#include <array>
#include <memory>
#include <vector>

class ActorInitializer;
class Bullet;
class BulletActor;
class BulletMLParser;
class BulletMLRunner;

class BulletActorPool : public ActorPool, public BulletManager {
public:
    BulletActorPool(int size, std::shared_ptr<ActorInitializer> initializer);

    void addBullet(float x, float y, float direction, float speed, BulletActor *parent);
    void addBullet(BulletMLState *state, float x, float y, float direction, float speed, BulletActor *parent);
    std::shared_ptr<BulletActor> addBullet(BulletMLParser *parser, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse);
    std::shared_ptr<BulletActor> addBullet(BulletMLParser *parser, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse, std::array<BulletMLParser *, MorphBullet::MORPH_MAX> morph, int morphSize, int morphCnt);

    void tick();
    int getTurn();
    void kill(Bullet *bullet);
    void clear();

private:
    int m_cnt;
};

#endif