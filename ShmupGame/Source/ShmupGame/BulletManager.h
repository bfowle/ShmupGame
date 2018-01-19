// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UBulletComponent.h"

#include <vector>

class BulletMLState;
class BulletMLParser;
class EnemyCommand;
class Bullet;
class UEnemyComponent;

extern int g_tick;

class SHMUPGAME_API BulletManager {
public:
	BulletManager(UEnemyComponent *enemyComponent);
	~BulletManager();

    EnemyCommand *createBullet(BulletMLParser *parser, Bullet *origin, UBulletComponent *target);
    EnemyCommand *createBullet(BulletMLState *state, double x, double y, double direction, double speed, UBulletComponent *target);
    Bullet *createProjectile(double x, double y, double direction, double speed);
    void destroyProjectile(Bullet *projectile);

    int getBulletCount() const { return m_bullets.size(); };

    void tick();

private:
    UEnemyComponent *m_enemyComponent;
    std::vector<EnemyCommand *> m_commands;
    std::vector<Bullet *> m_bullets;
    std::vector<Bullet *> m_pool;
};