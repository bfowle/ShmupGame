// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletManager.h"
#include "EnemyCommand.h"
#include "Bullet.h"

int g_tick;

BulletManager::BulletManager() {
    g_tick = 0;
}

BulletManager::~BulletManager() {
}

EnemyCommand *BulletManager::createBullet(BulletMLParser *parser, Bullet *origin, UMoverComponent *target) {
    UE_LOG(LogTemp, Warning, TEXT(" ][ create bullet (parser) ][ (%f, %f) (%f, %f) "), origin->getX(), origin->getY(), origin->getDirection(), origin->getSpeed());
    EnemyCommand *bc;
    bc = new EnemyCommand(parser, origin, target, this);
    m_commands.push_back(bc);
    return bc;
}

EnemyCommand *BulletManager::createBullet(BulletMLState *state, double x, double y, double direction, double speed, UMoverComponent *target) {
    UE_LOG(LogTemp, Warning, TEXT(" ][ create bullet (state) ][ (%f, %f) (%f, %f) "), x, y, direction, speed);
    Bullet *projectile = createProjectile(x, y, direction, speed);
    EnemyCommand *bc;
    bc = new EnemyCommand(state, projectile, target, this);
    m_commands.push_back(bc);
    return bc;
}

Bullet *BulletManager::createProjectile(double x, double y, double direction, double speed) {
    UE_LOG(LogTemp, Warning, TEXT(" ][ create projectile ][ (%f, %f) (%f, %f) "), x, y, direction, speed);
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
    bullet->setEnd(false);
    m_bullets.push_back(bullet);
    return bullet;
}

void BulletManager::destroyProjectile(Bullet *projectile) {
    projectile->setEnd(true);
}

void BulletManager::tick() {
    g_tick++;

    size_t size = m_bullets.size();
    if (size) {
        UE_LOG(LogTemp, Warning, TEXT(" ::tick -> bullets(%d)"), size);
    }
    for (size_t i = 0; i < size; ++i) {
        if (m_bullets[i]->isEnd()) {
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
    if (size) {
        UE_LOG(LogTemp, Warning, TEXT(" ::tick -> commands(%d)"), size);
    }
    for (size_t i = 0; i < size; ++i) {
        if (m_commands[i]->isEnd()) {
            delete m_commands[i];
            m_commands[i] = m_commands.back();
            m_commands.pop_back();
            --size;
        }
        if (i < size) {
            m_commands[i]->run();
            UE_LOG(LogTemp, Warning, TEXT(" ::tick -> m_commands[%d]->run() => %d "), i, m_commands[i]->getTurn());
        }
    }
}