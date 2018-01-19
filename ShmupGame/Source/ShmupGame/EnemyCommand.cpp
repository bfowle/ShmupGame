// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCommand.h"
#include "Bullet.h"
#include "BulletManager.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"
#include "bulletml/bulletmlrunner.h"

EnemyCommand::EnemyCommand(BulletMLParser *parser, Bullet *bullet, ABulletActor *target, BulletManager *owner) :
    BulletMLRunner(parser),
    m_parser(parser),
    m_bullet(bullet),
    m_target(target),
    m_owner(owner) {
}

EnemyCommand::EnemyCommand(BulletMLState *state, Bullet *bullet, ABulletActor *target, BulletManager *owner) :
    BulletMLRunner(state),
    m_state(state),
    m_bullet(bullet),
    m_target(target),
    m_owner(owner) {
}

EnemyCommand::~EnemyCommand() {
}

double EnemyCommand::getBulletDirection() {
    //UE_LOG(LogTemp, Warning, TEXT(" ===> getBulletDirection(%f) "), m_bullet->getDirection());
    return m_bullet->getDirection();
}

double EnemyCommand::getAimDirection() {
    UE_LOG(LogTemp, Warning, TEXT(" ===> getAimDirection() "));
    //return rtod(M_PI - atan2(mTarget->x - mMover->x, mTarget->y - mMover->y));
    return -1.0;
}

double EnemyCommand::getBulletSpeed() {
    //UE_LOG(LogTemp, Warning, TEXT(" ===> getBulletSpeed(%f) "), m_bullet->getSpeed());
    return m_bullet->getSpeed();
}

double EnemyCommand::getDefaultSpeed() {
    return 1.0;
}

double EnemyCommand::getRank() {
    return m_bullet->getRank();
}

void EnemyCommand::createSimpleBullet(double direction, double speed) {
    //UE_LOG(LogTemp, Warning, TEXT(" ][ CREATE SIMPLE BULLET | (%f, %f) ][ "), dtor(direction), speed);
    m_owner->createProjectile(m_bullet->getX(), m_bullet->getY(), dtor(direction), speed);
}

void EnemyCommand::createBullet(BulletMLState *state, double direction, double speed) {
    //UE_LOG(LogTemp, Warning, TEXT(" ][ CREATE BULLET | (%f, %f) | (%s) ][ "), dtor(direction), speed, state);
    m_owner->createBullet(state, m_bullet->getX(), m_bullet->getY(), dtor(direction), speed, m_target);
}

int EnemyCommand::getTurn() {
    return g_tick;
}

void EnemyCommand::doVanish() {
    //UE_LOG(LogTemp, Warning, TEXT(" => doVanish "));
}

void EnemyCommand::doChangeDirection(double direction) {
    m_bullet->setDirection(dtor(direction));
    //UE_LOG(LogTemp, Warning, TEXT(" => doChangeDirection(%f) [%f] "), direction, m_bullet->getDirection());
}

void EnemyCommand::doChangeSpeed(double speed) {
    m_bullet->setSpeed(speed);
    //UE_LOG(LogTemp, Warning, TEXT(" => doChangeSpeed(%f) [%f] "), speed, m_bullet->getSpeed());
}

void EnemyCommand::doAccelX(double accelX) {
    UE_LOG(LogTemp, Warning, TEXT(" => doAccelX(%f) "), accelX);
}

void EnemyCommand::doAccelY(double accelY) {
    UE_LOG(LogTemp, Warning, TEXT(" => doAccelY(%f) "), accelY);
}

double EnemyCommand::getBulletSpeedX() {
    UE_LOG(LogTemp, Warning, TEXT(" => getBulletSpeedX() "));
    return 0.0;
}

double EnemyCommand::getBulletSpeedY() {
    UE_LOG(LogTemp, Warning, TEXT(" => getBulletSpeedY() "));
    return 0.0;
}