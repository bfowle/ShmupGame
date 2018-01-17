// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletManager.h"
#include "BulletCommand.h"
#include "Bullet.h"

BulletManager::BulletManager() {
}

BulletManager::~BulletManager() {
}

BulletCommand *BulletManager::createBullet(BulletMLParser *parser, Bullet *origin, UMoverComponent *target) {
    BulletCommand *bc;
    bc = new BulletCommand(parser, origin, target, this);
    m_commands.push_back(bc);
    return bc;
}

BulletCommand *BulletManager::createBullet(BulletMLState *state, double x, double y, double d, double s, UMoverComponent *target) {
    Bullet *projectile = createProjectile(x, y, d, s);
    BulletCommand *bc;
    bc = new BulletCommand(state, projectile, target, this);
    m_commands.push_back(bc);
    return bc;
}

Bullet *BulletManager::createProjectile(double x, double y, double d, double s) {
    Bullet *bullet;
    if (m_pool.empty()) {
        bullet = new Bullet(x, y, d, s);
    } else {
        bullet = m_pool.back();
        //bullet->x = x;
        //bullet->y = y;
        //bullet->d = d;
        //bullet->s = s;
        m_pool.pop_back();
    }
    //bullet->dead = false;
    m_bullets.push_back(bullet);
    return bullet;
}

void BulletManager::destroyProjectile(Bullet *projectile) {
    //projectile->dead = true;
}

void BulletManager::tick() {
    unsigned int size = m_bullets.size();
    for (unsigned int i = 0; i < size; ++i) {
        //if (m_bullets[i]->dead) {
        //    m_pool.push_back(m_bullets[i]);
        //    m_bullets[i] = m_bullets.back();
        //    m_bullets.pop_back();
        //    --size;
        //}
        //if (i < size) {
        //    m_bullets[i]->tick();
        //}
    }

    size = m_commands.size();
    for (unsigned int i = 0; i < size; ++i) {
        //if (m_commands[i]->isDead()) {
        //    delete m_commands[i];
        //    m_commands[i] = m_commands.back();
        //    m_commands.pop_back();
        //    --size;
        //}
        //if (i < size) {
        //    m_commands[i]->run();
        //}
    }
}