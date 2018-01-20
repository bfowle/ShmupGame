// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletCommand.h"
#include "Movable.h"
#include "BulletManager.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"
#include "bulletml/bulletmlrunner.h"

BulletCommand::BulletCommand(BulletMLParser *parser, Movable *bullet, Movable *target, BulletManager *owner) :
    BulletMLRunner(parser),
    m_parser(parser),
    m_bullet(bullet),
    m_target(target),
    m_owner(owner) {
}

BulletCommand::BulletCommand(BulletMLState *state, Movable *bullet, Movable *target, BulletManager *owner) :
    BulletMLRunner(state),
    m_state(state),
    m_bullet(bullet),
    m_target(target),
    m_owner(owner) {
}

BulletCommand::~BulletCommand() {
}

double BulletCommand::getBulletDirection() {
    //UE_LOG(LogTemp, Warning, TEXT(" ===> getBulletDirection(%f) "), m_bullet->getDirection());
    return m_bullet->getDirection();
}

double BulletCommand::getAimDirection() {
    //UE_LOG(LogTemp, Warning, TEXT(" ===> getAimDirection() [t: %f, %f] == %f"), m_target->getX(), m_target->getY(),
    //  rtod(M_PI - atan2(m_target->getX() - m_bullet->getX(), m_target->getY() - m_bullet->getY())));
    return rtod(M_PI - atan2(m_target->getX() - m_bullet->getX(),
        m_target->getY() - m_bullet->getY()));
}

double BulletCommand::getBulletSpeed() {
    //UE_LOG(LogTemp, Warning, TEXT(" ===> getBulletSpeed(%f) "), m_bullet->getSpeed());
    return m_bullet->getSpeed();
}

double BulletCommand::getDefaultSpeed() {
    return 1.0;
}

double BulletCommand::getRank() {
    return m_bullet->getRank();
}

void BulletCommand::createSimpleBullet(double direction, double speed) {
    //UE_LOG(LogTemp, Warning, TEXT(" ][ CREATE SIMPLE BULLET | (%f, %f) ][ "), dtor(direction), speed);
    m_owner->createProjectile(m_bullet->getX(), m_bullet->getY(), dtor(direction), speed);
}

void BulletCommand::createBullet(BulletMLState *state, double direction, double speed) {
    //UE_LOG(LogTemp, Warning, TEXT(" ][ CREATE BULLET | (%f, %f) | (%s) ][ "), dtor(direction), speed, state);
    m_owner->createBullet(state, m_bullet->getX(), m_bullet->getY(), dtor(direction), speed, m_target);
}

int BulletCommand::getTurn() {
    return g_tick;
}

void BulletCommand::doVanish() {
    //UE_LOG(LogTemp, Warning, TEXT(" => doVanish "));
}

void BulletCommand::doChangeDirection(double direction) {
    m_bullet->setDirection(dtor(direction));
    //UE_LOG(LogTemp, Warning, TEXT(" => doChangeDirection(%f) [%f] "), direction, m_bullet->getDirection());
}

void BulletCommand::doChangeSpeed(double speed) {
    m_bullet->setSpeed(speed);
    //UE_LOG(LogTemp, Warning, TEXT(" => doChangeSpeed(%f) [%f] "), speed, m_bullet->getSpeed());
}

void BulletCommand::doAccelX(double accelX) {
    //UE_LOG(LogTemp, Warning, TEXT(" => doAccelX(%f) "), accelX);
    double sx = getBulletSpeedX();
    double sy = getBulletSpeedY();
    sx = accelX;
    m_bullet->setDirection(atan2(sy, sx));
    m_bullet->setSpeed(sqrt(sx*sx + sy * sy));
}

void BulletCommand::doAccelY(double accelY) {
    //UE_LOG(LogTemp, Warning, TEXT(" => doAccelY(%f) "), accelY);
    double sx = getBulletSpeedX();
    double sy = getBulletSpeedY();
    sy = accelY;
    m_bullet->setDirection(atan2(sy, sx));
    m_bullet->setSpeed(sqrt(sx*sx + sy * sy));
}

double BulletCommand::getBulletSpeedX() {
    //UE_LOG(LogTemp, Warning, TEXT(" => getBulletSpeedX() "));
    return m_bullet->getSpeed() * sin(m_bullet->getDirection());
}

double BulletCommand::getBulletSpeedY() {
    //UE_LOG(LogTemp, Warning, TEXT(" => getBulletSpeedY() "));
    return -m_bullet->getSpeed() * cos(m_bullet->getDirection());
}