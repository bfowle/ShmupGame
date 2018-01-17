// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "bulletml/bulletmlrunner.h"

class BulletMLParser;
class BulletMLState;
class BulletManager;
class Bullet;
class UMoverComponent;

class SHMUPGAME_API BulletCommand : public BulletMLRunner {
public:
    BulletCommand(BulletMLParser *parse, Bullet *bullet, UMoverComponent *target, BulletManager *owner);
    BulletCommand(BulletMLState *state, Bullet *bullet, UMoverComponent *target, BulletManager *owner);
    
    ~BulletCommand();

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
    Bullet *m_bullet;
    UMoverComponent *m_target;
    BulletManager *m_owner;
};
