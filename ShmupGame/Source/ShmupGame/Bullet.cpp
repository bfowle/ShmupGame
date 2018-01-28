#include "Bullet.h"

#include "BulletManager.h"

#include "bulletml/bulletml.h"
#include "bulletml/bulletmlrunner.h"

Bullet *Bullet::m_now;
FVector2D Bullet::m_target;
Random Bullet::m_random;
BulletManager *Bullet::m_manager = nullptr;

void Bullet::set(float x, float y, float direction, float speed, float rank) {
    m_position.X = x;
    m_position.Y = y;
    m_acceleration.X = 0;
    m_acceleration.Y = 0;
    m_direction = direction;
    m_speed = speed;
    m_rank = rank;
    //delete runner;
    m_runner = nullptr;
}

void Bullet::set(BulletMLRunner *runner, float x, float y, float direction, float speed, float rank) {
    set(x, y, direction, speed, rank);
    setRunner(runner);
}

void Bullet::setBulletManager(BulletManager *manager) {
    m_manager = manager;
    m_target = FVector2D(0, 0);
}

void Bullet::setRunner(BulletMLRunner *runner) {
    m_runner = runner;
}

void Bullet::addBullet(float direction, float speed) {
    m_manager->addBullet(direction, speed);
}

void Bullet::addBullet(BulletMLState *state, float direction, float speed) {
    m_manager->addBullet(state, direction, speed);
}

int Bullet::getTurn() {
    return m_manager->getTurn();
}

double Bullet::getRand() {
    return m_random.nextFloat(1);
}

void Bullet::tick() {
    m_now = this;

    //UE_LOG(LogTemp, Warning, TEXT("  -- Bullet::tick [%d, (%f, %f)]"), m_runner->getTurn(), m_direction, m_speed);

    if (!BulletMLRunner_isEnd(m_runner)) {
        BulletMLRunner_run(m_runner);
    }
}

bool Bullet::isEnd() {
    return BulletMLRunner_isEnd(m_runner);
}

void Bullet::kill() {
    m_manager->kill(this);
}

void Bullet::remove() {
    if (m_runner) {
        BulletMLRunner_delete(m_runner);
    }
}

const float VEL_SS_SDM_RATIO = 62.0 / 10;
const float VEL_SDM_SS_RATIO = 10.0 / 62;

double getBulletDirection_(BulletMLRunner *runner) {
    return rtod(Bullet::m_now->m_direction);
}

double getAimDirection_(BulletMLRunner *runner) {
    FVector2D b = Bullet::m_now->m_position;
    FVector2D t = Bullet::m_target;
    return rtod(atan2(t.X - b.X, t.Y - b.Y));
}

double getBulletSpeed_(BulletMLRunner *runner) {
    return Bullet::m_now->m_speed * VEL_SS_SDM_RATIO;
}

double getDefaultSpeed_(BulletMLRunner *runner) {
    return 1.0;
}

double getRank_(BulletMLRunner *runner) {
    return Bullet::m_now->m_rank;
}

void createSimpleBullet_(BulletMLRunner *runner, double d, double s) {
    Bullet::addBullet(dtor(d), s * VEL_SDM_SS_RATIO);
}

void createBullet_(BulletMLRunner *runner, BulletMLState* state, double d, double s) {
    Bullet::addBullet(state, dtor(d), s * VEL_SDM_SS_RATIO);
}

int getTurn_(BulletMLRunner *runner) {
    return Bullet::getTurn();
}

void doVanish_(BulletMLRunner *runner) {
    Bullet::m_now->kill();
}

void doChangeDirection_(BulletMLRunner *runner, double direction) {
    Bullet::m_now->m_direction = dtor(direction);
}

void doChangeSpeed_(BulletMLRunner *runner, double speed) {
    Bullet::m_now->m_speed = speed * VEL_SDM_SS_RATIO;
}

void doAccelX_(BulletMLRunner *runner, double accelX) {
    Bullet::m_now->m_acceleration.X = accelX * VEL_SDM_SS_RATIO;
}

void doAccelY_(BulletMLRunner *runner, double accelY) {
    Bullet::m_now->m_acceleration.Y = accelY * VEL_SDM_SS_RATIO;
}

double getBulletSpeedX_(BulletMLRunner *runner) {
    return Bullet::m_now->m_acceleration.X;
}

double getBulletSpeedY_(BulletMLRunner *runner) {
    return Bullet::m_now->m_acceleration.Y;
}

double getRand_(BulletMLRunner *runner) {
    return Bullet::getRand();
}