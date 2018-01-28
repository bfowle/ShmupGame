#ifndef BULLETACTOR_H
#define BULLETACTOR_H

#include "Actor.h"
#include "ActorInitializer.h"
#include "ShmupBullet.h"

#include "Math/Vector.h"

#include <array>
#include <memory>

class BulletMLParser;
class BulletMLRunner;
//class Ship;

class BulletActor : public Actor {
public:
    BulletActor() {};

private:
    BulletActor(const BulletActor &other);
    BulletActor &operator=(const BulletActor &other);

public:
    std::shared_ptr<Actor> newActor();

    static void init();
    void init(std::shared_ptr<ActorInitializer> initializer);
    void set(BulletMLRunner *runner, float x, float y, float direction, float speed, float rank, float speedRank, int shape, int color, int size, float xReverse);
    void set(BulletMLRunner *runner, float x, float y, float direction, float speed, float rank, float speedRank, int shape, int color, int size, float xReverse, std::array<BulletMLParser *, MorphBullet::MORPH_MAX> morph, int morphNum, int morphIdx, int morphCnt);
    void set(float x, float y, float direction, float speed, float rank, float speedRank, int shape, int color, int size, float xReverse);
    void setInvisible();
    void setTop(BulletMLParser *parser);
    //void rewind();
    void remove();
    void tick();

    static void resetTotalBulletsSpeed();

private:
    void start(float speedRank, int shape, int color, float size, float xReverse);
    void removeForced();
    //void checkShipHit();

public:
    enum {
        BULLET_SHAPE_NUM = 7,
        BULLET_COLOR_NUM = 4
    };

    std::shared_ptr<ShmupBullet> m_bullet;
    BulletMLParser *m_parser;
    static float m_totalBulletsSpeed;

private:
    enum {
        BULLET_DISAPPEAR_CNT = 180
    };

    //std::shared_ptr<Field> m_field;
    //std::shared_ptr<Ship> m_ship;

    static int m_nextId;
    bool m_isSimple;
    bool m_isTop;
    bool m_isVisible;

    FVector2D m_position;
    int m_cnt;
    bool m_shouldBeRemoved;
};

#endif