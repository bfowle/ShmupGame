// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UMoverComponent.h"

#include <vector>

class BulletMLState;
class BulletMLParser;
class BulletCommand;
class Bullet;

class SHMUPGAME_API BulletManager {
public:
	BulletManager();
	~BulletManager();

    BulletCommand *createBullet(BulletMLParser *parser, Bullet *origin, UMoverComponent *target);
    BulletCommand *createBullet(BulletMLState *state, double x, double y, double d, double s, UMoverComponent *target);
    Bullet *createProjectile(double x, double y, double d, double s);
    void destroyProjectile(Bullet *projectile);

    void tick();

private:
    std::vector<BulletCommand *> m_commands;
    std::vector<Bullet *> m_bullets;
    std::vector<Bullet *> m_pool;
};
