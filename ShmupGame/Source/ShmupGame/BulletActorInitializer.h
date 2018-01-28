#ifndef BULLETACTORINITIALIZER_H
#define BULLETACTORINITIALIZER_H

#include "ActorInitializer.h"

#include <memory>

//class Ship;

class BulletActorInitializer : public ActorInitializer {
public:
    BulletActorInitializer(/*std::shared_ptr<Field> field, std::shared_ptr<Ship> ship*/)/* :
        m_field(field),
        m_ship(ship)*/ {
    };

public:
    //std::shared_ptr<Field> m_field;
    //std::shared_ptr<Ship> m_ship;
};

#endif