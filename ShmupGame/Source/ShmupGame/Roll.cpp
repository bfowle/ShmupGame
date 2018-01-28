#include "Roll.h"

#include "RollInitializer.h"
#include "GameManager.h"

using namespace std;

const float Roll::BASE_LENGTH = 1.0;
const float Roll::BASE_RESISTANCE = 0.8;
const float Roll::BASE_SPRING = 0.2;
const float Roll::BASE_SIZE = 0.2;
const float Roll::BASE_DISTANCE = 3.0;
const float Roll::SPEED = 0.75;

shared_ptr<Actor> Roll::newActor() {
    return shared_ptr<Actor>(new Roll());
}

void Roll::init(shared_ptr<ActorInitializer> initializer) {
    shared_ptr<RollInitializer> ri = static_pointer_cast<RollInitializer>(initializer);

    //m_ship = ri->m_ship;
    //m_field = ri->m_field;
    m_gameManager = ri->m_gameManager;

    for (int i = 0; i < LENGTH; ++i) {
        m_position[i] = FVector2D();
        m_velocity[i] = FVector2D();
    }
}

void Roll::set() {
    for (int i = 0; i < LENGTH; ++i) {
        //m_position[i].X = m_ship->m_position.X;
        //m_position[i].Y = m_ship->m_position.Y;
        m_velocity[i].X = 0;
        m_velocity[i].Y = 0;
    }
    m_cnt = 0;
    m_distance = 0;
    m_released = false;
    m_exists = true;
}

void Roll::tick() {
    if (m_released) {
        m_position[0].Y += SPEED;
        //if (m_position[0].Y > m_field->m_size.Y) {
        //    m_exists = false;
        //    return;
        //}

        //m_gameManager->addParticle(...);
    } else {
        if (m_distance < BASE_DISTANCE) {
            m_distance += BASE_DISTANCE / 90;
        }
        //m_position[i].X = m_ship->m_position.X + sin(m_cnt * 0.1) * m_distance;
        //m_position[i].Y = m_ship->m_position.Y + cos(m_cnt * 0.1) * m_distance;
    }

    float dist, deg, v;
    for (int i = 1; i < LENGTH; ++i) {
        m_position[i].X += m_velocity[i].X;
        m_position[i].Y += m_velocity[i].Y;
        m_velocity[i].X *= BASE_RESISTANCE;
        m_velocity[i].Y *= BASE_RESISTANCE;

        dist = m_position[i].Distance(m_position[i], m_position[i - 1]);
        if (dist <= BASE_LENGTH) {
            continue;
        }

        v = (dist - BASE_LENGTH) * BASE_SPRING;
        deg = atan2(m_position[i - 1].X - m_position[i].X,
            m_position[i - 1].Y - m_position[i].Y);

        m_velocity[i].X += sin(deg) * v;
        m_velocity[i].Y += cos(deg) * v;
    }

    //UE_LOG(LogTemp, Warning, TEXT(" __ roll: %f, %f ; %f, %f __ "), m_position[0].X, m_position[0].Y, m_velocity[0].X, m_velocity[0].Y);

    m_cnt++;
}