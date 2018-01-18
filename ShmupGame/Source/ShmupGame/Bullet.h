// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class SHMUPGAME_API Bullet {
public:
    Bullet();
    Bullet(double x, double y, double direction, double speed);
    ~Bullet();

public:
    double getX() { return m_x; };
    double getY() { return m_y; };
    double getDirection() { return m_direction; };
    double getSpeed() { return m_speed; };
    double getRank() { return m_rank; }
    double isDead() { return m_isDead; };

    void setX(double x) { m_x = x; };
    void setY(double y) { m_y = y; };
    void setDirection(double direction) { m_direction = direction; };
    void setSpeed(double speed) { m_speed = speed; };
    void setDead() { m_isDead = true;  }

    void addNormalBullet(double direction, double speed);

    void tick();

private:
    double m_x, m_y;
    double m_direction, m_speed;
    double m_rank;
    bool m_isDead;
};
