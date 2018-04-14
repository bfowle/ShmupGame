#include "Bullet.h"

#include "BulletManager.h"

#include "bulletml/bulletml.h"
#include "bulletml/bulletmlrunner.h"
#include "bulletml/bulletmlparser.h"

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
    //UE_LOG(LogTemp, Warning, TEXT(" :: BULLET :: addBullet [%f, %f] "), direction, speed);
    m_manager->addBullet(direction, speed);
}

void Bullet::addBullet(BulletMLState *state, float direction, float speed) {
    //UE_LOG(LogTemp, Warning, TEXT(" :: BULLET :: addBullet[with state] [%f, %f] "), *state->getBulletML()->getName().c_str(), direction, speed);
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

    //UE_LOG(LogTemp, Warning, TEXT("  -- Bullet::tick [%d, (%f, %f)] :: [%s]"), m_runner->getTurn(),
    //    m_direction, m_speed, (isEnd() ? TEXT("Y") : TEXT("N")));

    if (!isEnd()) {
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
    if (m_runner != nullptr) {
        BulletMLRunner_delete(m_runner);
    }
}

//-----------------------------------------------------------------------------

const float VEL_SS_SDM_RATIO = 1.0; // 62.0 / 100;
const float VEL_SDM_SS_RATIO = 1.0; // 100.0 / 62;

float rtod(float a) {
    return a * 180 / M_PI;
}

float dtor(float a) {
    return a * M_PI / 180;
}

double getBulletDirection_(BulletMLRunner *runner) {
    return rtod(Bullet::m_now->m_direction);
}

double getAimDirection_(BulletMLRunner *runner) {
    return rtod(/*M_PI - */atan2(Bullet::m_target.X - Bullet::m_now->m_position.X,
        Bullet::m_target.Y - Bullet::m_now->m_position.Y));
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

void createSimpleBullet_(BulletMLRunner *runner, double direction, double speed) {
    //UE_LOG(LogTemp, Warning, TEXT(" 1.) createSimpleBullet :: ADD BULLET "));
    Bullet::addBullet(dtor(direction), speed * VEL_SDM_SS_RATIO);
}

void createBullet_(BulletMLRunner *runner, BulletMLState *state, double direction, double speed) {
    //UE_LOG(LogTemp, Warning, TEXT(" 2.) createBullet :: ADD ENEMY "));
    /*
    const vector<BulletMLNode *> &acts = state->getNode();
    vector<BulletMLNode *>::const_iterator ite;
    for (ite = acts.begin(); ite != acts.end(); ite++) {
        const BulletMLNode *action = *ite;
        if (action->findNode(BulletMLNode::fire) ||
            action->findNode(BulletMLNode::fireRef ||
            action->findNode(BulletMLNode::actionRef) {
            break;
        }
    }

    if (ite != acts.end()) {...}
    else {...}
    */
    Bullet::addBullet(state, dtor(direction), speed * VEL_SDM_SS_RATIO);
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
    return Bullet::m_now->m_acceleration.X * VEL_SS_SDM_RATIO;
}

double getBulletSpeedY_(BulletMLRunner *runner) {
    return Bullet::m_now->m_acceleration.Y * VEL_SS_SDM_RATIO;
}

double getRand_(BulletMLRunner *runner) {
    return Bullet::getRand();
}