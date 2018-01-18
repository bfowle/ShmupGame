// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletCommand.h"
#include "BulletManager.h"
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
    return (double)m_bullet->getDirection() * 360 / DIV;
}

double BulletCommand::getAimDirection() {
    //int d = getPlayerDeg(foe->pos.x, foe->pos.y);
    //if (foe->xReverse == -1) d = (-d) & 1023;
    //return ((double)d * 360 / DIV);
    return 0.0;
}

double BulletCommand::getBulletSpeed() {
    return ((double)m_bullet->getSpeed() / SCREEN_SPEED_RATE);
}

double BulletCommand::getDefaultSpeed() {
    return 1.0;
}

double BulletCommand::getRank() {
    return m_bullet->getRank();
}

void BulletCommand::createSimpleBullet(double direction, double speed) {
    UE_LOG(LogTemp, Warning, TEXT(" CREATE SIMPLE BULLET "));

    int d = (int)(direction * DIV / 360) & (DIV - 1);
    m_bullet->addNormalBullet(d, (int)(speed * SCREEN_SPEED_RATE));
    //m_bullet->fireCnt++;
}

void BulletCommand::createBullet(BulletMLState *state, double direction, double speed) {
    UE_LOG(LogTemp, Warning, TEXT(" CREATE BULLET "));
}

int BulletCommand::getTurn() {
    return g_tick;
}

void BulletCommand::doVanish() {
}

void BulletCommand::doChangeDirection(double direction) {
}

void BulletCommand::doChangeSpeed(double speed) {
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