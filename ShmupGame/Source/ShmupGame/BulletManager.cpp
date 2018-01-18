// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletManager.h"
#include "BulletCommand.h"
#include "Bullet.h"

int g_tick;

BulletManager::BulletManager() {
    g_tick = 0;
}

BulletManager::~BulletManager() {
}

BulletCommand *BulletManager::createBullet(BulletMLParser *parser, Bullet *origin, UMoverComponent *target) {
    BulletCommand *bc;
    bc = new BulletCommand(parser, origin, target, this);
    m_commands.push_back(bc);
    return bc;
}

BulletCommand *BulletManager::createBullet(BulletMLState *state, double x, double y, double direction, double speed,UMoverComponent *target) {
    Bullet *projectile = createProjectile(x, y, direction, speed);
    BulletCommand *bc;
    bc = new BulletCommand(state, projectile, target, this);
    m_commands.push_back(bc);
    return bc;
}

Bullet *BulletManager::createProjectile(double x, double y, double direction, double speed) {
    Bullet *bullet;
    if (m_pool.empty()) {
        bullet = new Bullet(x, y, direction, speed);
    } else {
        bullet = m_pool.back();
        bullet->setX(x);
        bullet->setY(y);
        bullet->setDirection(direction);
        bullet->setSpeed(speed);
        m_pool.pop_back();
    }
    bullet->setDead();
    m_bullets.push_back(bullet);
    return bullet;
}

void BulletManager::destroyProjectile(Bullet *projectile) {
    projectile->setDead();
}

void BulletManager::tick() {
    g_tick++;

    size_t size = m_bullets.size();
    for (size_t i = 0; i < size; ++i) {
        if (m_bullets[i]->isDead()) {
            m_pool.push_back(m_bullets[i]);
            m_bullets[i] = m_bullets.back();
            m_bullets.pop_back();
            --size;
        }
        if (i < size) {
            m_bullets[i]->tick();
        }
    }

    size = m_commands.size();
    for (size_t i = 0; i < size; ++i) {
        if (m_commands[i]->isEnd()) {
            delete m_commands[i];
            m_commands[i] = m_commands.back();
            m_commands.pop_back();
            --size;
        }
        if (i < size) {
            m_commands[i]->run();
        }
    }
}