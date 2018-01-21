// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class SHMUPGAME_API Movable {
public:
    Movable();
    Movable(double x, double y, double direction, double speed);
    ~Movable();

public:
    double getX() const { return m_x; };
    double getY() const { return m_y; };
    double getDirection() const { return m_direction; };
    double getSpeed() const { return m_speed; };
    double getRank() const { return m_rank; }
    double isEnd() const { return m_isEnd; };

    void setX(double x) { m_x = x; };
    void setY(double y) { m_y = y; };
    void setDirection(double direction) { m_direction = direction; };
    void setSpeed(double speed) { m_speed = speed; };
    void setEnd(bool isEnd) { m_isEnd = isEnd;  }

    void tick();

protected:
    double m_x, m_y;
    double m_direction, m_speed;
    //double m_vx, m_vy;
    double m_rank;
    bool m_isEnd;
};
