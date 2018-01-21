// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletCommand.h"
#include "Movable.h"
#include "BulletManager.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"
#include "bulletml/bulletmlrunner.h"

BulletCommand::BulletCommand(BulletMLParser *parser, Movable *movable, Movable *target, BulletManager *owner) :
    BulletMLRunner(parser),
    m_parser(parser),
    m_movable(movable),
    m_target(target),
    m_owner(owner),
    m_isDead(false) {
}

BulletCommand::BulletCommand(BulletMLState *state, Movable *movable, Movable *target, BulletManager *owner) :
    BulletMLRunner(state),
    m_state(state),
    m_movable(movable),
    m_target(target),
    m_owner(owner),
    m_isDead(false) {
}

BulletCommand::~BulletCommand() {
}

double BulletCommand::getBulletDirection() {
    return m_movable->getDirection();
}

double BulletCommand::getAimDirection() {
    return rtod(M_PI - atan2(m_target->getX() - m_movable->getX(),
        m_target->getY() - m_movable->getY()));
}

double BulletCommand::getBulletSpeed() {
    return m_movable->getSpeed();
}

double BulletCommand::getDefaultSpeed() {
    return 1.0;
}

double BulletCommand::getRank() {
    return m_movable->getRank();
}

void BulletCommand::createSimpleBullet(double direction, double speed) {
    m_owner->createProjectile(m_movable->getX(), m_movable->getY(),
        dtor(direction), speed);
}

void BulletCommand::createBullet(BulletMLState *state, double direction, double speed) {
    m_owner->createBullet(state, m_movable->getX(), m_movable->getY(),
        dtor(direction), speed, m_target);
}

int BulletCommand::getTurn() {
    return g_tick;
}

void BulletCommand::doVanish() {
    m_isDead = true;
}

void BulletCommand::doChangeDirection(double direction) {
    m_movable->setDirection(dtor(direction));
}

void BulletCommand::doChangeSpeed(double speed) {
    m_movable->setSpeed(speed);
}

void BulletCommand::doAccelX(double accelX) {
    double sy = getBulletSpeedY();
    m_movable->setDirection(atan2(sy, accelX));
    m_movable->setSpeed(sqrt(accelX * accelX + sy * sy));
}

void BulletCommand::doAccelY(double accelY) {
    double sx = getBulletSpeedX();
    m_movable->setDirection(atan2(accelY, sx));
    m_movable->setSpeed(sqrt(sx * sx + accelY * accelY));
}

double BulletCommand::getBulletSpeedX() {
    return m_movable->getSpeed() *
        sin(m_movable->getDirection());
}

double BulletCommand::getBulletSpeedY() {
    return -m_movable->getSpeed() *
        cos(m_movable->getDirection());
}
