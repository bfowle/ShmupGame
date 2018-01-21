// Fill out your copyright notice in the Description page of Project Settings.

#include "BulletManager.h"

#include "BulletCommand.h"
#include "Movable.h"
#include "UMovableComponentBase.h"
#include "UBulletComponent.h"

// @TODO: put this elsewhere
int g_tick = 0;

BulletManager::BulletManager(UMovableComponentBase *owner) :
    m_owner(owner) {
}

BulletManager::~BulletManager() {
}

BulletCommand *BulletManager::createBullet(BulletMLParser *parser, Movable *origin, Movable *target) {
    //UE_LOG(LogTemp, Warning, TEXT(" ][ create bullet (parser) ][ (%f, %f) (%f, %f) "), origin->getX(), origin->getY(), origin->getDirection(), origin->getSpeed());
    BulletCommand *bc = new BulletCommand(parser, origin, target, this);
    m_commands.push_back(bc);
    return bc;
}

BulletCommand *BulletManager::createBullet(BulletMLState *state, double x, double y, double direction, double speed, Movable *target) {
    //UE_LOG(LogTemp, Warning, TEXT(" ][ create bullet (state) => %s ][ (%f, %f) (%f, %f) "), state, x, y, direction, speed);
    Movable *projectile = createProjectile(x, y, direction, speed);
    BulletCommand *bc = new BulletCommand(state, projectile, target, this);
    m_commands.push_back(bc);
    return bc;
}

Movable *BulletManager::createProjectile(double x, double y, double direction, double speed) {
    //UE_LOG(LogTemp, Warning, TEXT(" ][ create projectile ][ (%f, %f) (%f, %f) "), x, y, direction, speed);
    UBulletComponent *bullet;
    if (m_pool.empty()) {
        bullet = m_owner->spawnBulletActor(x, y, direction, speed);
    } else {
        bullet = m_pool.back();
        bullet->setX(x);
        bullet->setY(y);
        bullet->setDirection(direction);
        bullet->setSpeed(speed);
        m_pool.pop_back();
    }
    m_bullets.push_back(bullet);
    return bullet;
}

void BulletManager::destroyProjectile(Movable *projectile) {
    projectile->setEnd(true);
}

void BulletManager::tick() {
    g_tick++;

    size_t size = m_bullets.size();
    //UE_LOG(LogTemp, Warning, TEXT(" ::tick -> m_bullets.size() == %d"), m_bullets.size());
    for (size_t i = 0; i < size; ++i) {
        if (m_bullets[i]->isEnd()) {
            if (m_bullets[i]->GetOwner() != nullptr) {
                //m_bullets[i]->GetOwner()->DestroyConstructedComponents();
                m_bullets[i]->GetOwner()->Destroy(true, true);
            }

            m_pool.push_back(m_bullets[i]);
            m_bullets[i] = m_bullets.back();
            m_bullets.pop_back();
            --size;
        }
        if (i < size) {
            //UE_LOG(LogTemp, Warning, TEXT(" ::tick -> m_bullets[%d]->tick() "), i);
            m_bullets[i]->tick();
        }
    }

    size = m_commands.size();
    //UE_LOG(LogTemp, Warning, TEXT(" ::tick -> m_commands.size() == %d"), m_commands.size());
    for (size_t i = 0; i < size; ++i) {
        if (m_commands[i]->isDead()) {
            delete m_commands[i];
            m_commands[i] = m_commands.back();
            m_commands.pop_back();
            --size;
        }
        if (i < size) {
            //UE_LOG(LogTemp, Warning, TEXT(" ::tick -> m_commands[%d]->run() => %d "), i, m_commands[i]->getTurn());
            m_commands[i]->run();
        }
    }
}