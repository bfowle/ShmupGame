#ifndef LOCKINITIALIZER_H
#define LOCKINITIALIZER_H

#include "ActorInitializer.h"

class AGameManager;

class LockInitializer : public ActorInitializer {
public:
    LockInitializer(/*std::shared_ptr<Ship> ship, std::shared_ptr<Field> field, */AGameManager *gameManager) :
        m_gameManager(gameManager) {
    }

public:
    //std::shared_ptr<Ship> m_ship;
    //std::shared_ptr<Field> m_field;
    AGameManager *m_gameManager;
};

#endif