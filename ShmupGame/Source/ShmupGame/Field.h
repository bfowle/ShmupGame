#ifndef FIELD_H
#define FIELD_H

#include "Math/Vector.h"

#include <array>

class AGameManager;

class Field {
public:
    void init(FVector2D size);
    void tick();
    bool checkHit(const FVector2D &position);
    bool checkHit(const FVector2D &position, float space);

public:
    FVector2D m_size;
};

#endif