#ifndef BULLETMANAGER_H
#define BULLETMANAGER_H

//#include "CoreMinimal.h"

#include <bulletml/bulletmlrunner.h>

#include <vector>

class Bullet;

class BulletManager {
public:
    virtual ~BulletManager() {};

    virtual void addBullet(float direction, float speed) = 0;
    virtual void addBullet(BulletMLState *state, float direction, float speed) = 0;
    virtual int getTurn() = 0;
    virtual void kill(Bullet *bullet) = 0;
};

/*
class BulletMLState;
class BulletMLParser;
class BulletCommand;
class Movable;
class UMovableComponentBase;
class UBulletComponent;

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
    std::vector<UBulletComponent *> m_bullets;
    std::vector<UBulletComponent *> m_pool;
};
*/

#endif