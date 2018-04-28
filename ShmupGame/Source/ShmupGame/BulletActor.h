#ifndef BULLETACTOR_H
#define BULLETACTOR_H

#include "Actor.h"
#include "ShmupBullet.h"

#include "Math/Vector.h"

#include <array>
#include <memory>

class AGameManager;
class Field;
class Ship;
class BulletCommand;
class BulletMLParser;
class BulletMLRunner;

class BulletActorInitializer : public ActorInitializer {
public:
    BulletActorInitializer(std::shared_ptr<Field> field, std::shared_ptr<Ship> ship, TWeakObjectPtr<AGameManager> gameManager) :
        m_field(field),
        m_ship(ship),
        m_gameManager(gameManager) {
    };

public:
    std::shared_ptr<Field> m_field;
    std::shared_ptr<Ship> m_ship;
    TWeakObjectPtr<AGameManager> m_gameManager;
};

class BulletActor : public Actor {
public:
    BulletActor() :
        SHIP_HIT_WIDTH(0.2) {
    };

private:
    BulletActor(const BulletActor &other);
    BulletActor &operator=(const BulletActor &other);

public:
    std::shared_ptr<Actor> newActor();

    static void init();
    void init(std::shared_ptr<ActorInitializer> initializer);
    void set(BulletCommand *command, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse);
    void set(BulletCommand *command, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse, std::array<BulletMLParser *, MorphBullet::MORPH_MAX> morph, int morphSize, int morphIdx, int morphCnt);
    void set(float x, float y, float direction, float speed, float rank, float speedRank, float xReverse);
    void setInvisible();
    void setTop(BulletMLParser *parser);
    void spawnBulletActor();
    void rewind();
    void remove();
    void tick();

    static void resetTotalBulletsSpeed();

private:
    void start(float speedRank, float xReverse);
    void removeForced();

public:
    enum {
        BULLET_SHAPE_NUM = 7,
        BULLET_COLOR_NUM = 4
    };

    int32 m_instanceId;
    std::shared_ptr<ShmupBullet> m_bullet;

    static float m_totalBulletsSpeed;

private:
    enum {
        BULLET_DISAPPEAR_COUNT = 180
    };

    const float SHIP_HIT_WIDTH;
    static const float FIELD_SPACE;

    std::shared_ptr<Field> m_field;
    std::shared_ptr<Ship> m_ship;
    TWeakObjectPtr<AGameManager> m_gameManager;
    BulletMLParser *m_parser;

    static int m_nextId;
    bool m_isSimple;
    bool m_isTop;
    bool m_isVisible;

    FVector m_previousPosition;
    bool m_shouldBeRemoved;
};

#endif