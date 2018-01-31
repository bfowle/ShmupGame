#ifndef BULLETACTORINITIALIZER_H
#define BULLETACTORINITIALIZER_H

#include "ActorInitializer.h"

#include <memory>

class AGameManager;
class Field;
class Ship;

class BulletActorInitializer : public ActorInitializer {
public:
    BulletActorInitializer(std::shared_ptr<Field> field, std::shared_ptr<Ship> ship, TWeakObjectPtr<AGameManager> gameManager) :
        m_field(field),
        m_ship(ship),
        m_gameManager(gameManager) {
    };

public:
    std::shared_ptr<Field> m_field;
    std::shared_ptr<Ship> m_ship;
    TWeakObjectPtr<AGameManager> m_gameManager;
};

#endif