// Fill out your copyright notice in the Description page of Project Settings.

#include "Bullet.h"

/**
 * @TODO: rename to something like Enemy inherit from Movable superclass
*/

Bullet::Bullet() {
}

Bullet::Bullet(double x, double y, double direction, double speed) :
    m_x(x),
    m_y(y),
    m_direction(direction),
    m_speed(speed),
    m_rank(0.0) {
}

Bullet::~Bullet() {
}

void Bullet::addNormalBullet(double direction, double speed) {
}

void Bullet::tick() {
}