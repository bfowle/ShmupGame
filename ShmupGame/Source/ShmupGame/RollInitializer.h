#ifndef ROLLINITIALIZER_H
#define ROLLINITIALIZER_H

#include "ActorInitializer.h"

class AGameManager;

class RollInitializer : public ActorInitializer {
public:
    RollInitializer(/*std::shared_ptr<Ship> ship, std::shared_ptr<Field> field, */AGameManager *gameManager) :
        m_gameManager(gameManager) {
    }

public:
    //std::shared_ptr<Ship> m_ship;
    //std::shared_ptr<Field> m_field;
    AGameManager *m_gameManager;
};

#endif