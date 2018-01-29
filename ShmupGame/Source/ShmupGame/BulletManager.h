#ifndef BULLETMANAGER_H
#define BULLETMANAGER_H

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

#endif