// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyBullet.h"
#include "Bullet.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"
#include "bulletml/bulletmlrunner.h"

EnemyBullet::EnemyBullet(BulletMLParser *parser, Bullet *bullet) : BulletMLRunner(parser) {
    m_bullet = bullet;
}

EnemyBullet::EnemyBullet(BulletMLState *state, Bullet *bullet) : BulletMLRunner(state) {
    m_bullet = bullet;
}

EnemyBullet::~EnemyBullet() {
}

double EnemyBullet::getBulletDirection() {
    return 0.0;
}

double EnemyBullet::getAimDirection() {
    return 0.0;
}

double EnemyBullet::getBulletSpeed() {
    return 0.0;
}

double EnemyBullet::getDefaultSpeed() {
    return 0.0;
}

double EnemyBullet::getRank() {
    return 0.0;
}

void EnemyBullet::createSimpleBullet(double direction, double speed) {
}

void EnemyBullet::createBullet(BulletMLState *state, double direction, double speed) {
}

int EnemyBullet::getTurn() {
    return 0;
}

void EnemyBullet::doVanish() {
}

void EnemyBullet::doChangeDirection(double direction) {
}

void EnemyBullet::doChangeSpeed(double sspeed) {
}

void EnemyBullet::doAccelX(double accelX) {
}

void EnemyBullet::doAccelY(double accelY) {
}

double EnemyBullet::getBulletSpeedX() {
    return 0.0;
}

double EnemyBullet::getBulletSpeedY() {
    return 0.0;
}