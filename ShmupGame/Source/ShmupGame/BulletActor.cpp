#include "BulletActor.h"

#include "BulletActor.h"
#include "BulletActorPool.h"
#include "GameManager.h"
#include "Field.h"
#include "Ship.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "bulletml/bulletml.h"
#include "bulletml/bulletmlparser.h"

using namespace std;

const float BulletActor::FIELD_SPACE = 15.0; // 0.5;
float BulletActor::m_totalBulletsSpeed;
int BulletActor::m_nextId = 0;

shared_ptr<Actor> BulletActor::newActor() {
    return shared_ptr<Actor>(new BulletActor());
}

void BulletActor::init() {
    m_nextId = 0;
}

void BulletActor::init(shared_ptr<ActorInitializer> initializer) {
    shared_ptr<BulletActorInitializer> bullet = static_pointer_cast<BulletActorInitializer>(initializer);
    m_field = bullet->m_field;
    m_ship = bullet->m_ship;
    m_gameManager = bullet->m_gameManager;

    m_bullet.reset(new ShmupBullet(m_nextId));
    m_previousPosition = FVector2D();
    ++m_nextId;
}

// called via bulletml createBullet
void BulletActor::set(BulletMLRunner *runner, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse) {
    m_bullet->set(runner, x, y, direction, speed, rank);
    m_bullet->m_isMorph = false;
    m_isSimple = false;
    start(speedRank, xReverse);
}

// called via bulletml createSimpleBullet as a morph bullet
void BulletActor::set(BulletMLRunner *runner, float x, float y, float direction, float speed, float rank, float speedRank, float xReverse, array<BulletMLParser *, MorphBullet::MORPH_MAX> morph, int morphSize, int morphIdx, int morphCnt) {
    //UE_LOG(LogTemp, Warning, TEXT(" BulletActor::set => morph bullet "));
    m_bullet->set(runner, x, y, direction, speed, rank);
    m_bullet->setMorph(morph, morphSize, morphIdx, morphCnt);
    m_isSimple = false;
    start(speedRank, xReverse);
}

// called via bulletml createSimpleBullet
void BulletActor::set(float x, float y, float direction, float speed, float rank, float speedRank, float xReverse) {
    m_bullet->set(x, y, direction, speed, rank);
    m_bullet->m_isMorph = false;
    m_isSimple = true;
    start(speedRank, xReverse);
}

void BulletActor::start(float speedRank, float xReverse) {
    m_isAlive = true;
    m_isTop = false;
    m_isVisible = true;
    m_previousPosition.X = m_bullet->m_position.X;
    m_previousPosition.Y = m_bullet->m_position.Y;
    m_bullet->setParam(speedRank, xReverse);
    m_cnt = 0;
    m_rtCnt = 0;
    m_shouldBeRemoved = false;
    m_backToRetro = false;
}

void BulletActor::setInvisible() {
    m_isVisible = false;
}

void BulletActor::setTop(BulletMLParser *parser) {
    m_parser = parser;
    m_isTop = true;
    setInvisible();
}

void BulletActor::spawnBulletActor() {
    m_actor = m_gameManager->m_world->SpawnActor<AActor>(m_gameManager->BP_BulletClass,
        FVector(m_bullet->m_position.X, 100.0, m_bullet->m_position.Y), FRotator::ZeroRotator);

    if (m_actor.IsValid()) {
        m_uuid = m_actor->GetUniqueID();
        m_movement = m_actor->FindComponentByClass<UProjectileMovementComponent>();
    }
}

void BulletActor::rewind() {
    m_bullet->remove();

    BulletMLRunner *runner = BulletMLRunner_new_parser(m_parser);
    BulletActorPool::registerFunctions(runner);
    m_bullet->setRunner(runner);
    m_bullet->resetMorph();
}

void BulletActor::remove() {
    m_shouldBeRemoved = true;
}

void BulletActor::removeForced() {
    m_bullet->remove();
    m_isAlive = false;

    if (m_actor.IsValid()) {
        m_gameManager->m_world->DestroyActor(m_actor.Get());
        //m_actor->Destroy();
        //m_actor->ConditionalBeginDestroy();
    }
}

void BulletActor::toRetro() {
    if (!m_isVisible ||
        m_backToRetro) {
        return;
    }
    m_backToRetro = true;

    if (m_rtCnt >= RETRO_COUNT) {
        m_rtCnt = RETRO_COUNT - 0.1;
    }
}

void BulletActor::tick() {
    m_previousPosition.X = m_bullet->m_position.X;
    m_previousPosition.Y = m_bullet->m_position.Y;

    if (!m_isSimple) {
        m_bullet->tick();

        if (m_isTop &&
            m_bullet->isEnd()) {
            rewind();
        }
    }

    if (m_shouldBeRemoved) {
        removeForced();
        return;
    }

    float sr = 0;
#if 1
    if (m_rtCnt < RETRO_COUNT) {
        sr = m_bullet->m_speedRank * (0.3 + (m_rtCnt / RETRO_COUNT) * 0.7);
        if (m_backToRetro) {
            m_rtCnt -= sr;
            if (m_rtCnt <= 0) {
                removeForced();
                return;
            }
        } else {
            m_rtCnt += sr;
        }

        if (m_ship->m_cnt < -m_ship->INVINCIBLE_COUNT / 2 &&
            m_isVisible &&
            m_rtCnt >= RETRO_COUNT) {
            removeForced();
            return;
        }
    } else {
        sr = m_bullet->m_speedRank;
        if (m_cnt > BULLET_DISAPPEAR_COUNT) {
            toRetro();
        }
    }
#else
    sr = m_bullet->m_speedRank;
#endif

    //m_bullet->m_position.X += (sin(m_bullet->m_direction) * m_bullet->m_speed + m_bullet->m_acceleration.X) * sr * m_bullet->m_xReverse;
    //m_bullet->m_position.Y += (cos(m_bullet->m_direction) * m_bullet->m_speed - m_bullet->m_acceleration.Y) * sr;

    if (m_isVisible) {
        m_totalBulletsSpeed += m_bullet->m_speed * sr;

        //if (m_rtCnt > RETRO_COUNT) {
        //    //checkShipHit();
        //}

        //if (m_field->checkHit(m_bullet->m_position, FIELD_SPACE)) {
        //    //UE_LOG(LogTemp, Warning, TEXT(" __ Bullet :: hit field __ [%s] ... %d "), *m_bullet->m_position.ToString(), m_cnt);
        //    removeForced();
        //}

        if (m_actor.IsValid() && 
            m_movement.IsValid() &&
            m_movement->UpdatedComponent) {
            FVector vel = (m_movement->UpdatedComponent->GetForwardVector() *
                (sin(m_bullet->m_direction) +
                    m_bullet->m_acceleration.X) * sr *
                    m_bullet->m_xReverse) +
                (m_movement->UpdatedComponent->GetUpVector() *
                (cos(m_bullet->m_direction) -
                    m_bullet->m_acceleration.Y) * sr);
            vel.Y = 0;
            if (!vel.IsNearlyZero()) {
                vel.Normalize();
                vel *= m_bullet->m_speed *
                    m_movement->GetMaxSpeed() *
                    m_gameManager->m_deltaSeconds;
                m_movement->MoveUpdatedComponent(vel, FRotator::ZeroRotator, true);
                if (m_movement.IsValid()) {
                    m_movement->UpdateComponentVelocity();
                }
            }

            if (m_actor.IsValid()) {
                m_bullet->m_position.X = m_actor->GetActorLocation().X;
                m_bullet->m_position.Y = m_actor->GetActorLocation().Z;
            }
        }
    }

    m_cnt++;
}

void BulletActor::resetTotalBulletsSpeed() {
    m_totalBulletsSpeed = 0;
}