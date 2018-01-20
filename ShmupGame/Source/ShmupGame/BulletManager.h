// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <vector>

class BulletMLState;
class BulletMLParser;
class BulletCommand;
class Movable;
class UMovableComponentBase;

extern int g_tick;

class SHMUPGAME_API BulletManager {
public:
	BulletManager(UMovableComponentBase *owner);
	~BulletManager();

    BulletCommand *createBullet(BulletMLParser *parser, Movable *origin, Movable *target);
    BulletCommand *createBullet(BulletMLState *state, double x, double y, double direction, double speed, Movable *target);
    Movable *createProjectile(double x, double y, double direction, double speed);
    void destroyProjectile(Movable *projectile);

    int getBulletCount() const { return m_bullets.size(); };

    void tick();

private:
    UMovableComponentBase *m_owner;
    std::vector<BulletCommand *> m_commands;
    std::vector<Movable *> m_bullets;
    std::vector<Movable *> m_pool;
};
