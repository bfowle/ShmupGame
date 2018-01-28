#ifndef ROLLINITIALIZER_H
#define ROLLINITIALIZER_H

#include "ActorInitializer.h"

class AGameManager;
class Field;
class Ship;

class RollInitializer : public ActorInitializer {
public:
    RollInitializer(std::shared_ptr<Field> field, std::shared_ptr<Ship> ship, AGameManager *gameManager) :
        m_field(field),
        m_ship(ship),
        m_gameManager(gameManager) {
    }

public:
    std::shared_ptr<Field> m_field;
    std::shared_ptr<Ship> m_ship;
    AGameManager *m_gameManager;
};

#endif