// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletCommand.h"
#include "Bullet.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"
#include "bulletml/bulletmlrunner.h"

BulletCommand::BulletCommand(BulletMLParser *parser, Bullet *bullet, UMoverComponent *target, BulletManager *owner) :
    BulletMLRunner(parser),
    m_bullet(bullet),
    m_target(target),
    m_owner(owner) {
}

BulletCommand::BulletCommand(BulletMLState *state, Bullet *bullet, UMoverComponent *target, BulletManager *owner) :
    BulletMLRunner(state),
    m_bullet(bullet),
    m_target(target),
    m_owner(owner) {
}

BulletCommand::~BulletCommand() {
}

double BulletCommand::getBulletDirection() {
    return 0.0;
}

double BulletCommand::getAimDirection() {
    return 0.0;
}

double BulletCommand::getBulletSpeed() {
    return 0.0;
}

double BulletCommand::getDefaultSpeed() {
    return 0.0;
}

double BulletCommand::getRank() {
    return 0.0;
}

void BulletCommand::createSimpleBullet(double direction, double speed) {
}

void BulletCommand::createBullet(BulletMLState *state, double direction, double speed) {
}

int BulletCommand::getTurn() {
    return 0;
}

void BulletCommand::doVanish() {
}

void BulletCommand::doChangeDirection(double direction) {
}

void BulletCommand::doChangeSpeed(double sspeed) {
}

void BulletCommand::doAccelX(double accelX) {
}

void BulletCommand::doAccelY(double accelY) {
}

double BulletCommand::getBulletSpeedX() {
    return 0.0;
}

double BulletCommand::getBulletSpeedY() {
    return 0.0;
}