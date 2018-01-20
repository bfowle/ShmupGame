// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "bulletml/bulletmlrunner.h"

#define _USE_MATH_DEFINES
#include <math.h>

// @TODO: put these elsewhere
//#define DIV 1024
//#define SCREEN_SPEED_RATE 512
//#define SCREEN_VELOCITY_RATE 512

class BulletMLParser;
class BulletMLState;
class BulletManager;
class Movable;

class SHMUPGAME_API BulletCommand : public BulletMLRunner {
public:
    BulletCommand(BulletMLParser *parser, Movable *bullet, Movable *target, BulletManager *owner);
    BulletCommand(BulletMLState *state, Movable *bullet, Movable *target, BulletManager *owner);
    
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

    bool isDead() { return m_isDead || isEnd(); };

private:
    BulletMLParser *m_parser;
    BulletMLState *m_state;
    Movable *m_movable;
    Movable *m_target;
    BulletManager *m_owner;

    bool m_isDead;

    inline double dtor(double d) { return d * M_PI / 180.0; };
    inline double rtod(double r) { return r * 180.0 / M_PI; };
};
