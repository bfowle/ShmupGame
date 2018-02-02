#ifndef BULLET_H
#define BULLET_H

#include "Random.h"

#include "Math/Vector.h"

#define _USE_MATH_DEFINES
#include <math.h>

class BulletManager;
class BulletMLRunner;
class BulletMLState;

class Bullet {
public:
    explicit Bullet(int id) :
        m_id(id),
        m_runner(nullptr) {
    };
    virtual ~Bullet() {};

private:
    Bullet(const Bullet &);
    Bullet &operator=(const Bullet &);

public:
    void set(float x, float y, float direction, float speed, float rank);
    void set(BulletMLRunner *runner, float x, float y, float direction, float speed, float rank);

    static void setBulletManager(BulletManager *manager);
    void setRunner(BulletMLRunner *runner);

    static void addBullet(float direction, float speed);
    static void addBullet(BulletMLState *state, float direction, float speed);
    static int getTurn();
    static double getRand();

    void tick();
    bool isEnd();
    void kill();
    void remove();

public:
    int m_id;

    static Bullet *m_now;
    static FVector2D m_target;
    FVector2D m_position, m_acceleration;
    float m_direction, m_speed, m_rank;

private:
    static Random m_random;
    static BulletManager *m_manager;
    BulletMLRunner *m_runner;
};

float rtod(float a);
float dtor(float a);

extern "C" {
    double getBulletDirection_(BulletMLRunner *runner);
    double getAimDirection_(BulletMLRunner *runner);
    double getBulletSpeed_(BulletMLRunner *runner);
    double getDefaultSpeed_(BulletMLRunner *runner);
    double getRank_(BulletMLRunner *runner);
    void createSimpleBullet_(BulletMLRunner *runner, double direction, double speed);
    void createBullet_(BulletMLRunner *runner, BulletMLState *state, double direction, double speed);
    int getTurn_(BulletMLRunner *runner);
    void doVanish_(BulletMLRunner *runner);
    void doChangeDirection_(BulletMLRunner *runner, double direction);
    void doChangeSpeed_(BulletMLRunner *runner, double speed);
    void doAccelX_(BulletMLRunner *runner, double accelX);
    void doAccelY_(BulletMLRunner *runner, double accelY);
    double getBulletSpeedX_(BulletMLRunner *runner);
    double getBulletSpeedY_(BulletMLRunner *runner);
    double getRand_(BulletMLRunner *r);
}

#endif