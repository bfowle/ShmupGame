#ifndef BULLETACTOR_H
#define BULLETACTOR_H

#include "Actor.h"
#include "ActorInitializer.h"
#include "ShmupBullet.h"

#include "Math/Vector.h"

#include <array>
#include <memory>

class AActor;
class AGameManager;
class Field;
class Ship;
class BulletMLParser;
class BulletMLRunner;

class BulletActor : public Actor {
public:
    BulletActor() :
        m_actorSpawned(false),
        SHIP_HIT_WIDTH(0.2),
        RETRO_COUNT(24) {
    };

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
    void spawnBulletActor();
    void rewind();
    void remove();
    void toRetro();
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
    static float m_totalBulletsSpeed;

private:
    enum {
        BULLET_DISAPPEAR_COUNT = 180
    };

    const float SHIP_HIT_WIDTH;
    const float RETRO_COUNT;
    static const float FIELD_SPACE;
    //static const float m_shapePos[][8][2];
    //static const float m_bulletColor[4][3];
    //static const float SHAPE_POINT_SIZE;
    //static const float SHAPE_BASE_COLOR_R;
    //static const float SHAPE_BASE_COLOR_G;
    //static const float SHAPE_BASE_COLOR_B;

    TWeakObjectPtr<AActor> m_actor;
    std::shared_ptr<Field> m_field;
    std::shared_ptr<Ship> m_ship;
    TWeakObjectPtr<AGameManager> m_gameManager;
    BulletMLParser *m_parser;

    static int m_nextId;
    bool m_isSimple;
    bool m_isTop;
    bool m_isVisible;

    FVector2D m_previousPosition;
    int m_cnt;
    float m_rtCnt;
    bool m_actorSpawned;
    bool m_shouldBeRemoved;
    bool m_backToRetro;
};

#endif