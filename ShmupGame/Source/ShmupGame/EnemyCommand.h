// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "bulletml/bulletmlrunner.h"

#define DIV 1024
#define SCREEN_SPEED_RATE 512
#define SCREEN_VELOCITY_RATE 512

class BulletMLParser;
class BulletMLState;
class BulletManager;
class Bullet;
class UMoverComponent;

class SHMUPGAME_API EnemyCommand : public BulletMLRunner {
public:
    EnemyCommand(BulletMLParser *parser, Bullet *bullet, UMoverComponent *target, BulletManager *owner);
    EnemyCommand(BulletMLState *state, Bullet *bullet, UMoverComponent *target, BulletManager *owner);
    
    ~EnemyCommand();

public:
    virtual double getBulletDirection();
    virtual double getAimDirection();
    virtual double getBulletSpeed();
    virtual double getDefaultSpeed();
    virtual double getRank();
    virtual void createSimpleBullet(double direction, double speed);
    virtual void createBullet(BulletMLState *state, double direction, double speed);
    virtual int getTurn();
    virtual void doVanish();

    virtual void doChangeDirection(double direction);
    virtual void doChangeSpeed(double speed);
    virtual void doAccelX(double accelX);
    virtual void doAccelY(double accelY);
    virtual double getBulletSpeedX();
    virtual double getBulletSpeedY();

private:
    BulletMLParser *m_parser;
    BulletMLState *m_state;
    Bullet *m_bullet;
    UMoverComponent *m_target;
    BulletManager *m_owner;
};
