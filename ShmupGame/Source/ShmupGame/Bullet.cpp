#include "Bullet.h"

#include "BulletManager.h"

#include "bulletml/bulletml.h"

FVector Bullet::m_target;
Random Bullet::m_random;
BulletManager *Bullet::m_manager = nullptr;

void Bullet::set(BulletActor *actor, float x, float y, float direction, float speed, float rank) {
    m_actor = actor;
    m_position.X = x;
    m_position.Y = 2502.0; // @TODO: hardcode this to 0 eventually
    m_position.Z = y;
    m_acceleration.X = 0;
    m_acceleration.Z = 0;
    m_direction = direction;
    m_speed = speed;
    m_rank = rank;
}

void Bullet::set(BulletActor *actor, BulletCommand *command, float x, float y, float direction, float speed, float rank) {
    set(actor, x, y, direction, speed, rank);
    m_command.reset(command);
}

void Bullet::setBulletManager(BulletManager *manager) {
    m_manager = manager;
    m_target = FVector(0, 0, 0);
}

void Bullet::addBullet(float direction, float speed) {
    m_manager->addBullet(m_position.X, m_position.Z, direction, speed, m_actor);
}

void Bullet::addBullet(BulletMLState *state, float direction, float speed) {
    m_manager->addBullet(state, m_position.X, m_position.Z, direction, speed, m_actor);
}

int Bullet::getTurn() {
    return m_manager->getTurn();
}

double Bullet::getRand() {
    return m_random.nextFloat(1);
}

void Bullet::tick() {
    //UE_LOG(LogTemp, Warning, TEXT("  -- Bullet::tick [%d, (%f, %f)] :: [%s]"), m_runner->getTurn(),
    //    m_direction, m_speed, (isEnd() ? TEXT("Y") : TEXT("N")));

    if (m_command.get() != nullptr) {
        m_command->run();

        if (m_command->isEnd()) {
            m_command.reset(nullptr);
            //endBullet();
        }
    }
}

void Bullet::endBullet() {
    m_speed = 0;
}

bool Bullet::isEnd() {
    return m_command->isEnd();
}

void Bullet::kill() {
    m_manager->kill(this);
}

void Bullet::remove() {
    if (m_command.get() != nullptr) {
        m_command.reset(nullptr);
    }
}

//-----------------------------------------------------------------------------

const float VEL_SS_SDM_RATIO = 62.0 / 100;
const float VEL_SDM_SS_RATIO = 100.0 / 62;

float rtod(float a) {
    return a * 180 / M_PI;
}

float dtor(float a) {
    return a * M_PI / 180;
}

BulletCommand::BulletCommand(BulletMLParser *parser, Bullet *bullet) :
    BulletMLRunner(parser),
    m_bullet(bullet) {
}

BulletCommand::BulletCommand(BulletMLParser *parser, BulletMLNode *node, Bullet *bullet) :
    BulletMLRunner(parser),
    m_bullet(bullet) {
}

BulletCommand::BulletCommand(BulletMLState *state, Bullet *bullet) :
    BulletMLRunner(state),
    m_bullet(bullet) {
}

double BulletCommand::getBulletDirection() {
    return rtod(m_bullet->m_direction);
}

double BulletCommand::getAimDirection() {
    return rtod(/*M_PI - */atan2(Bullet::m_target.X - m_bullet->m_position.X,
        Bullet::m_target.Z - m_bullet->m_position.Z));
}

double BulletCommand::getBulletSpeed() {
    return m_bullet->m_speed * VEL_SS_SDM_RATIO;
}

double BulletCommand::getDefaultSpeed() {
    return 1.0;
}

double BulletCommand::getRank() {
    return m_bullet->m_rank;
}

void BulletCommand::createSimpleBullet(double direction, double speed) {
    m_bullet->addBullet(dtor(direction), speed * VEL_SDM_SS_RATIO);
}

void BulletCommand::createBullet(BulletMLState *state, double direction, double speed) {
    m_bullet->addBullet(state, dtor(direction), speed * VEL_SDM_SS_RATIO);
}

int BulletCommand::getTurn() {
    return Bullet::getTurn();
}

void BulletCommand::doVanish() {
    m_bullet->kill();
}

void BulletCommand::doChangeDirection(double direction) {
    m_bullet->m_direction = dtor(direction);
}

void BulletCommand::doChangeSpeed(double speed) {
    m_bullet->m_speed = speed * VEL_SDM_SS_RATIO;
}

void BulletCommand::doAccelX(double accelX) {
    m_bullet->m_acceleration.X = accelX * VEL_SDM_SS_RATIO;
}

void BulletCommand::doAccelY(double accelY) {
    m_bullet->m_acceleration.Z = accelY * VEL_SDM_SS_RATIO;
}

double BulletCommand::getBulletSpeedX() {
    return m_bullet->m_acceleration.X * VEL_SS_SDM_RATIO;
}

double BulletCommand::getBulletSpeedY() {
    return m_bullet->m_acceleration.Z * VEL_SS_SDM_RATIO;
}

double BulletCommand::getRand() {
    return Bullet::getRand();
}