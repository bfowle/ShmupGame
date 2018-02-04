#include "GameManager.h"

#include "StageManager.h"
#include "BarrageManager.h"
#include "BulletActor.h"
#include "BulletActorInitializer.h"
#include "BulletActorPool.h"
#include "Enemy.h"
#include "EnemyInitializer.h"
#include "Field.h"
#include "Lock.h"
#include "LockInitializer.h"
#include "Roll.h"
#include "RollInitializer.h"
#include "Ship.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

#include "bulletml/bulletmlparser.h"

using namespace std;

const int AGameManager::SLOWDOWN_START_BULLETS_SPEED[2] = {30, 42};

AGameManager::AGameManager() {
    // @TODO: move this to AGameState?
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.bCanEverTick = true;
}

// called before any other scripts (including PreInitializeComponents)
// used by AGameModeBase to initialize parameters and spawn its helper classes
void AGameManager::InitGame(const FString &MapName, const FString &Options, FString &ErrorMessage) {
    Super::InitGame(MapName, Options, ErrorMessage);

    m_random = Random();
    m_world = GetWorld();

    m_field.reset(new Field());
    m_field->init();

    m_ship.reset(new Ship());
    m_ship->init(m_field, this);

    shared_ptr<BulletActorInitializer> bi(new BulletActorInitializer(m_field, m_ship, this));
    m_bullets.reset(new BulletActorPool(512, bi));

    unique_ptr<Roll> rollClass(new Roll());
    shared_ptr<RollInitializer> ri(new RollInitializer(m_field, m_ship, this));
    m_rolls.reset(new ActorPool(4, rollClass.get(), ri));

    Lock::init();
    unique_ptr<Lock> lockClass(new Lock());
    shared_ptr<LockInitializer> li(new LockInitializer(m_field, m_ship, this));
    m_locks.reset(new ActorPool(4, lockClass.get(), li));

    unique_ptr<Enemy> m_enemyClass(new Enemy());
    shared_ptr<EnemyInitializer> ei(new EnemyInitializer(m_field, m_ship, m_bullets, m_rolls, m_locks, this));
    m_enemies.reset(new ActorPool(ENEMY_MAX, m_enemyClass.get(), ei));

    m_barrageManager.reset(new BarrageManager());
    m_barrageManager->loadBulletMLFiles();
    EnemyType::init(m_barrageManager);

    m_stageManager.reset(new StageManager());
    m_stageManager->init(m_field, m_barrageManager, this);

    //m_interval = INTERVAL_BASE;
}

void AGameManager::StartPlay() {
    Super::StartPlay();

    TWeakObjectPtr<APawn> player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (player.IsValid()) {
        m_ship->setPlayerPawn(player);
    }

    //startTitle();
    //m_mode = LOCK;
    //m_difficulty = EXTREME;
    startInGame();
}

void AGameManager::Tick(float DeltaSeconds) {
    Super::Tick(DeltaSeconds);

    m_deltaSeconds = DeltaSeconds;

    switch (m_state) {
    case TITLE:
        titleTick();
        break;
    case IN_GAME:
        inGameTick();
        break;
    case GAME_OVER:
        gameOverTick();
        break;
    case PAUSE:
        pauseTick();
        break;
    default:
        break;
    }

    m_cnt++;
}

void AGameManager::AddShot(const FVector &position, float direction) {
    UE_LOG(LogTemp, Warning, TEXT("FIRING SHOT: %s | %f"), *position.ToString(), direction);
}

void AGameManager::RemoveEnemy(AActor *enemy) {
    vector<shared_ptr<Actor>>::iterator it = find_if(m_enemies->m_actor.begin(), m_enemies->m_actor.end(),
        [&](shared_ptr<Actor> a) { return a->m_uuid == enemy->GetUniqueID(); });
    if (it != m_enemies->m_actor.end()) {
        //UE_LOG(LogTemp, Warning, TEXT("Remove Enemy :: %s --- %d"), *enemy->GetName(), enemy->GetUniqueID());
        (*it)->remove();
    }
}

void AGameManager::RemoveBullet(AActor *bullet) {
    vector<shared_ptr<Actor>>::iterator it = find_if(m_bullets->m_actor.begin(), m_bullets->m_actor.end(),
        [&](shared_ptr<Actor> a) { return a->m_uuid == bullet->GetUniqueID(); });
    if (it != m_bullets->m_actor.end()) {
        //UE_LOG(LogTemp, Warning, TEXT("Remove Bullet :: %s --- %d"), *bullet->GetName(), bullet->GetUniqueID());
        (*it)->remove();
    }
}

void AGameManager::close() {
    m_barrageManager->unloadBulletMLFiles();
}

void AGameManager::startStage(int difficulty, int parsecSlot, int startParsec, int mode) {
    m_enemies->clear();
    m_bullets->clear();

    m_difficulty = difficulty;
    m_parsecSlot = parsecSlot;
    m_mode = mode;

    int stageType = m_random.nextInt(99999);

    switch (difficulty) {
    case PRACTICE:
        m_stageManager->setRank(1, 4, startParsec, stageType);
        m_ship->setSpeedRate(0.7);
        break;
    case NORMAL:
        m_stageManager->setRank(10, 8, startParsec, stageType);
        m_ship->setSpeedRate(0.9);
        break;
    case HARD:
        m_stageManager->setRank(22, 12, startParsec, stageType);
        m_ship->setSpeedRate(1);
        break;
    case EXTREME:
        m_stageManager->setRank(36, 16, startParsec, stageType);
        m_ship->setSpeedRate(1.2);
        break;
    case QUIT:
        m_stageManager->setRank(0, 0, 0, 0);
        m_ship->setSpeedRate(1);
        break;
    }
}

void AGameManager::addEnemy(const FVector2D &position, float direction, shared_ptr<EnemyType> type, BulletMLParser *moveParser) {
    shared_ptr<Enemy> enemy = static_pointer_cast<Enemy>(m_enemies->getInstance());
    if (!enemy) {
        return;
    }

    enemy->set(position, direction, type, moveParser);

    if (enemy->shouldSpawnActor()) {
        enemy->setActor(m_world->SpawnActor<AActor>(BP_EnemyClass,
            FVector(position.X, 100.0, position.Y), FRotator::ZeroRotator));
    }
}

void AGameManager::addBoss(const FVector2D &position, float direction, shared_ptr<EnemyType> type) {
}

void AGameManager::addRoll() {
    shared_ptr<Roll> roll = static_pointer_cast<Roll>(m_rolls->getInstance());
    if (roll) {
        roll->set();
    }
}

void AGameManager::addLock() {
    shared_ptr<Lock> lock = static_pointer_cast<Lock>(m_locks->getInstance());
    if (lock) {
        lock->set();
    }
}

void AGameManager::releaseRoll() {
    for (int i = 0; i < m_rolls->m_actor.size(); ++i) {
        if (!m_rolls->m_actor[i]->m_exists) {
            continue;
        }
        static_pointer_cast<Roll>(m_rolls->m_actor[i])->m_released = true;
    }
}

void AGameManager::releaseLock() {
    for (int i = 0; i < m_locks->m_actor.size(); ++i) {
        if (!m_locks->m_actor[i]->m_exists) {
            continue;
        }
        static_pointer_cast<Lock>(m_locks->m_actor[i])->m_released = true;
    }
}

void AGameManager::shipDestroyed() {
    if (m_mode == ROLL) {
        releaseRoll();
    } else {
        releaseLock();
    }
    clearBullets();

    m_shipsRemaining--;
    if (m_shipsRemaining < 0) {
        startGameOver();
    }
}

void AGameManager::clearBullets() {
    for (int i = 0; i < m_bullets->m_actor.size(); i++) {
        if (!m_bullets->m_actor[i]->m_exists) {
            continue;
        }
        //static_pointer_cast<BulletActor>(m_bullets->m_actor[i]->toRetro());
    }
}

void AGameManager::initShipState() {
    m_shipsRemaining = 2;
    m_score = 0;
    m_extendScore = FIRST_EXTEND;
    m_ship->start();
}

void AGameManager::startInGame() {
    m_state = IN_GAME;

    initShipState();

    int startParsec = m_parsecSlot * 10 + 1;
    startStage(m_difficulty, m_parsecSlot, startParsec/*m_title->getStartParsec(m_difficulty, m_parsecSlot)*/, m_mode);
}

void AGameManager::startTitle() {
    m_state = TITLE;
    //m_title->start();

    initShipState();
    m_bullets->clear();
    m_ship->m_cnt = 0;
    m_cnt = 0;

    int startParsec = m_parsecSlot * 10 + 1;
    startStage(m_difficulty, m_parsecSlot, startParsec/*m_title->getStartParsec(m_difficulty, m_parsecSlot)*/, m_mode);
}

void AGameManager::startGameOver() {
    m_state = GAME_OVER;

    m_shots->clear();
    m_rolls->clear();
    m_locks->clear();
    
    m_cnt = 0;
}

void AGameManager::startPause() {
    m_state = PAUSE;

    m_pauseCnt = 0;
}

void AGameManager::resumePause() {
    m_state = IN_GAME;
}

void AGameManager::stageTick() {
    m_stageManager->tick();
}

void AGameManager::inGameTick() {
    /*
    float nowTick = UGameplayStatics::GetRealTimeSeconds(GetWorld()) * 1000.0;
    m_frame = (int)(nowTick - m_previousTick) / m_interval;
    if (m_frame <= 0) {
        m_frame = 1;
        m_previousTick += m_interval;
    } else if (m_frame > m_maxSkipFrame) {
        m_frame = m_maxSkipFrame;
        m_previousTick = nowTick;
    } else {
        m_previousTick += m_frame * m_interval;
    }
    for (int i = 0; i < m_frame; i++) {
        stageTick();
    }
    */
    stageTick();

    m_field->tick();
    m_ship->tick();
    //m_shots->tick();
    m_enemies->tick();

    if (m_mode == ROLL) {
        m_rolls->tick();
    } else {
        m_locks->tick();
    }

    BulletActor::resetTotalBulletsSpeed();
    m_bullets->tick();

    // set pause

    /*
    if (BulletActor::m_totalBulletsSpeed > SLOWDOWN_START_BULLETS_SPEED[m_mode]) {
        float sm = BulletActor::m_totalBulletsSpeed / SLOWDOWN_START_BULLETS_SPEED[m_mode];
        if (sm > 1.75) {
            sm = 1.75;
        }
        m_interval += (sm * INTERVAL_BASE - m_interval) * 0.01;
    } else {
        m_interval += (INTERVAL_BASE - m_interval) * 0.08;
    }
    */
}

void AGameManager::titleTick() {
}

void AGameManager::gameOverTick() {
}

void AGameManager::pauseTick() {
}