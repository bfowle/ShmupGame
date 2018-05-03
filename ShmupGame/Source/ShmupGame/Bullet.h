#ifndef BULLET_H
#define BULLET_H

#include "Random.h"

#include "Math/Vector.h"

#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>

#include "bulletml/bulletmlrunner.h"
#include "bulletml/bulletmlparser.h"

class BulletActor;
class BulletCommand;
class BulletManager;
class BulletMLState;

class Bullet {
public:
    explicit Bullet(int id) :
        m_id(id),
        m_rank(1.0) {
    };
    virtual ~Bullet() {};

private:
    Bullet(const Bullet &);
    Bullet &operator=(const Bullet &);

public:
    void set(BulletActor *actor, float x, float y, float direction, float speed, float rank);
    void set(BulletActor *actor, BulletCommand *command, float x, float y, float direction, float speed, float rank);

    static void setBulletManager(BulletManager *manager);

    void addBullet(float direction, float speed);
    void addBullet(BulletMLState *state, float direction, float speed);
    static int getTurn();
    static double getRand();

    void tick();
    void endBullet();
    bool isEnd();
    void kill();
    void remove();

public:
    int m_id;

    static FVector m_target;
    FVector m_position, m_acceleration;
    float m_direction, m_speed, m_rank;

protected:
    std::unique_ptr<BulletCommand> m_command;
    BulletMLState *m_state;

    friend class BulletCommand;

private:
    static Random m_random;
    static BulletManager *m_manager;
    BulletActor *m_actor;
};

////////////////////////////////////////////////////////////////////////////////

class BulletCommand : public BulletMLRunner {
public:
    BulletCommand(BulletMLParser *parser, Bullet *bullet);
    BulletCommand(BulletMLParser *parser, BulletMLNode *node, Bullet *bullet);
    BulletCommand(BulletMLState *state, Bullet *bullet);

public:
    virtual void run() {
        BulletMLRunner::run();
    }
    virtual bool isEnd() const {
        return BulletMLRunner::isEnd();
    }

protected:
    virtual double getBulletDirection();
    virtual double getAimDirection();
    virtual double getBulletSpeed();
    virtual double getDefaultSpeed();
    virtual double getRank();
    virtual void createSimpleBullet(double direction, double speed);
    virtual void createBullet(BulletMLState *state, double direction, double speed);
    virtual int getTurn();
    virtual void doVanish();
    virtual void doChangeDirection(double direction);
    virtual void doChangeSpeed(double speed);
    virtual void doAccelX(double accelX);
    virtual void doAccelY(double accelY);
    virtual double getBulletSpeedX();
    virtual double getBulletSpeedY();

    virtual double getRand();

private:
    Bullet *m_bullet;
};

constexpr float rtod(float a) noexcept {
    return a * 180 / M_PI;
}

constexpr float dtor(float a) noexcept {
    return a * M_PI / 180;
}

#endif