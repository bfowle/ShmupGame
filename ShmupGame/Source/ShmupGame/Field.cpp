#include "Field.h"

#include "GameManager.h"

using namespace std;

void Field::init(FVector2D size) {
    m_size = size;
}

void Field::tick() {
}

bool Field::checkHit(const FVector2D &position) {
    return (position.X < -m_size.X ||
        position.X > m_size.X ||
        position.Y < -m_size.Y ||
        position.Y > m_size.Y);
}

bool Field::checkHit(const FVector2D &position, float space) {
    return (position.X < -m_size.X + space ||
        position.X > m_size.X - space ||
        position.Y < -m_size.Y + space ||
        position.Y > m_size.Y - space);
}