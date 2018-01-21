// Fill out your copyright notice in the Description page of Project Settings.

#include "Movable.h"

Movable::Movable() :
    m_x(0.0),
    m_y(0.0),
    m_direction(0.0),
    m_speed(0.0),
    m_rank(1.0) {
}

Movable::Movable(double x, double y, double direction, double speed) :
    m_x(x),
    m_y(y),
    m_direction(direction),
    m_speed(speed),
    m_rank(1.0) {
}

Movable::~Movable() {
}

void Movable::tick() {
    m_x += m_speed * sin(m_direction);
    m_y -= m_speed * cos(m_direction);

    //UE_LOG(LogTemp, Warning, TEXT(" Bullet::tick() (%f, %f) | [%f, %f] "), m_x, m_y, m_direction, m_speed);
}