#ifndef LOCKINITIALIZER_H
#define LOCKINITIALIZER_H

#include "ActorInitializer.h"

class AGameManager;
class Field;
class Ship;

class LockInitializer : public ActorInitializer {
public:
    LockInitializer(std::shared_ptr<Field> field, std::shared_ptr<Ship> ship, AGameManager *gameManager) :
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