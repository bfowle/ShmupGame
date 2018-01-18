// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCommand.h"
#include "Bullet.h"
#include "BulletManager.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"
#include "bulletml/bulletmlrunner.h"

EnemyCommand::EnemyCommand(BulletMLParser *parser, Bullet *bullet, UMoverComponent *target, BulletManager *owner) :
    BulletMLRunner(parser),
    m_parser(parser),
    m_bullet(bullet),
    m_target(target),
    m_owner(owner) {
    UE_LOG(LogTemp, Warning, TEXT(" ~~~~~ EnemyCommand::parser constructor ~~~~~"));
}

EnemyCommand::EnemyCommand(BulletMLState *state, Bullet *bullet, UMoverComponent *target, BulletManager *owner) :
    BulletMLRunner(state),
    m_state(state),
    m_bullet(bullet),
    m_target(target),
    m_owner(owner) {
    UE_LOG(LogTemp, Warning, TEXT(" ~~~~~ EnemyCommand::state constructor ~~~~~"));
}

EnemyCommand::~EnemyCommand() {
}

double EnemyCommand::getBulletDirection() {
    UE_LOG(LogTemp, Warning, TEXT(" ===> getBulletDirection(%f) "), ((double)m_bullet->getDirection() * 360 / DIV));
    return (double)m_bullet->getDirection() * 360 / DIV;
}

double EnemyCommand::getAimDirection() {
    //int d = getPlayerDeg(foe->pos.x, foe->pos.y);
    //if (foe->xReverse == -1) d = (-d) & 1023;
    //return ((double)d * 360 / DIV);
    return 0.0;
}

double EnemyCommand::getBulletSpeed() {
    UE_LOG(LogTemp, Warning, TEXT(" ===> getBulletSpeed(%f) "), ((double)m_bullet->getSpeed() / SCREEN_SPEED_RATE));
    return ((double)m_bullet->getSpeed() / SCREEN_SPEED_RATE);
}

double EnemyCommand::getDefaultSpeed() {
    return 1.0;
}

double EnemyCommand::getRank() {
    return m_bullet->getRank();
}

void EnemyCommand::createSimpleBullet(double direction, double speed) {
    double d = (double)((int)(direction * DIV / 360) & (DIV - 1));
    UE_LOG(LogTemp, Warning, TEXT(" ][ CREATE SIMPLE BULLET | (%f, %f) | (%f) ][ "), direction, (speed * SCREEN_SPEED_RATE), d);
    m_owner->createProjectile(m_bullet->getX(), m_bullet->getY(), d, (speed * SCREEN_SPEED_RATE));
    //m_bullet->fireCnt++;
}

void EnemyCommand::createBullet(BulletMLState *state, double direction, double speed) {
    double d = (double)((int)(direction * DIV / 360) & (DIV - 1));
    UE_LOG(LogTemp, Warning, TEXT(" ][ CREATE BULLET | (%f, %f) | (%f) | (%s) ][ "), direction, (speed * SCREEN_SPEED_RATE), d, state);
    m_owner->createBullet(state, m_bullet->getX(), m_bullet->getY(), d, (speed * SCREEN_SPEED_RATE), m_target);
}

int EnemyCommand::getTurn() {
    return g_tick;
}

void EnemyCommand::doVanish() {
    UE_LOG(LogTemp, Warning, TEXT(" => doVanish "));
}

void EnemyCommand::doChangeDirection(double direction) {
    m_bullet->setDirection(direction * DIV / 360.0);
    UE_LOG(LogTemp, Warning, TEXT(" => doChangeDirection(%f) [%f] "), direction, m_bullet->getDirection());
}

void EnemyCommand::doChangeSpeed(double speed) {
    m_bullet->setSpeed(speed * SCREEN_SPEED_RATE);
    UE_LOG(LogTemp, Warning, TEXT(" => doChangeSpeed(%f) [%f] "), speed, m_bullet->getSpeed());
}

void EnemyCommand::doAccelX(double accelX) {
    //m_bullet->vel.x = (int)(accelX * SCREEN_VELOCITY_RATE);
    UE_LOG(LogTemp, Warning, TEXT(" => doAccelX(%f) "), accelX);
}

void EnemyCommand::doAccelY(double accelY) {
    //m_bullet->vel.y = (int)(accelY * SCREEN_VELOCITY_RATE);
    UE_LOG(LogTemp, Warning, TEXT(" => doAccelY(%f) "), accelY);
}

double EnemyCommand::getBulletSpeedX() {
    return 0.0;
    //return ((double)m_bullet->vel.x / SCREEN_VELOCITY_RATE);
}

double EnemyCommand::getBulletSpeedY() {
    return 0.0;
    //return ((double)m_bullet->vel.y / SCREEN_VELOCITY_RATE);
}