// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UMoverComponent.h"

#include <vector>

class BulletMLState;
class BulletMLParser;
class EnemyCommand;
class Bullet;

extern int g_tick;

class SHMUPGAME_API BulletManager {
public:
	BulletManager();
	~BulletManager();

    EnemyCommand *createBullet(BulletMLParser *parser, Bullet *origin, UMoverComponent *target);
    EnemyCommand *createBullet(BulletMLState *state, double x, double y, double direction, double speed, UMoverComponent *target);
    Bullet *createProjectile(double x, double y, double direction, double speed);
    void destroyProjectile(Bullet *projectile);

    void tick();

private:
    std::vector<EnemyCommand *> m_commands;
    std::vector<Bullet *> m_bullets;
    std::vector<Bullet *> m_pool;
};
