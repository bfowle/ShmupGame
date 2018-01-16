// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "bulletml/bulletmlrunner.h"

class BulletMLParser;
class BulletMLState;
class Bullet;

class SHMUPGAME_API EnemyBullet : public BulletMLRunner {
public:
    EnemyBullet(BulletMLParser *parse, Bullet *bullet);
    EnemyBullet(BulletMLState *state, Bullet *bullet);
    
    ~EnemyBullet();

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
};
