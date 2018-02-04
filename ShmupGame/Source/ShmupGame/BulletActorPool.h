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
    BulletActorPool(int n, std::shared_ptr<ActorInitializer> initializer);

    void addBullet(float direction, float speed);
    void addBullet(BulletMLState *state, float direction, float speed);
    std::shared_ptr<BulletActor> addBullet(BulletMLRunner *runner, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse);
    std::shared_ptr<BulletActor> addBullet(BulletMLParser *parser, BulletMLRunner *runner, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse);
    std::shared_ptr<BulletActor> addBullet(BulletMLParser *parser, BulletMLRunner *runner, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse, std::array<BulletMLParser *, MorphBullet::MORPH_MAX> morph, int morphSize, int morphCnt);

    void tick();
    int getTurn();
    void kill(Bullet *bullet);
    void clear();

    static void registerFunctions(BulletMLRunner *runner);

private:
    int m_cnt;
};

#ifdef __cplusplus
extern "C" {
#endif
    double getAimDirectionWithXRev_(BulletMLRunner *runner);
#ifdef __cplusplus
}
#endif

#endif