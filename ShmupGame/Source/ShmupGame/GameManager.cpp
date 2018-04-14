#include "GameManager.h"

#include "StageManager.h"
#include "BarrageManager.h"
#include "BulletActor.h"
#include "BulletActorPool.h"
#include "Enemy.h"
#include "Field.h"
#include "Ship.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

#include "bulletml/bulletml.h"
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
    m_bullets.reset(new BulletActorPool(BULLET_MAX, bi));

    unique_ptr<Enemy> m_enemyClass(new Enemy());
    shared_ptr<EnemyInitializer> ei(new EnemyInitializer(m_field, m_ship, m_bullets, this));
    m_enemies.reset(new ActorPool(ENEMY_MAX, m_enemyClass.get(), ei));

    m_barrageManager.reset(new BarrageManager());
    m_barrageManager->loadBulletMLFiles();
    EnemyType::init(m_barrageManager);

    m_stageManager.reset(new StageManager());
    m_stageManager->init(m_field, m_barrageManager, this);

    //m_interval = INTERVAL_BASE;
    //m_maxSkipFrame = 5;
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

    //m_cnt++;
}

void AGameManager::AddShot(const FVector &position, float direction) {
    UE_LOG(LogTemp, Warning, TEXT("FIRING SHOT: %s | %f"), *position.ToString(), direction);
}

void AGameManager::AddEnemy(AActor *actor, FString moveFilePath) {
    shared_ptr<Enemy> enemy = static_pointer_cast<Enemy>(m_enemies->getInstance());
    if (!enemy) {
        return;
    }

    // @TODO: !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // get new squadron from StageManager
    StageManager::EnemySquadron *squadron = &m_stageManager->m_squadrons[0];
    BulletMLParserTinyXML *moveParser = BulletMLParserTinyXML_new(const_cast<char *>(TCHAR_TO_UTF8(*moveFilePath)));
    BulletMLParserTinyXML_parse(moveParser);

    enemy->set(FVector2D(actor->GetActorLocation().X, actor->GetActorLocation().Z),
        M_PI, squadron->m_type, reinterpret_cast<BulletMLParser *>(moveParser));

    /*
    TWeakObjectPtr<APawn> player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (player.IsValid() &&
        player->GetAttachParentActor()) {
        actor->AttachToActor(player->GetAttachParentActor(),
            FAttachmentTransformRules::KeepWorldTransform);
        //actor->SetActorRelativeLocation(FVector(position.X, 0, position.Y));
    }
    */
    enemy->setActor(actor);
}

void AGameManager::RemoveEnemy(AActor *enemy) {
    vector<shared_ptr<Actor>>::iterator it = find_if(m_enemies->m_pool.begin(), m_enemies->m_pool.end(),
        [&](shared_ptr<Actor> a) { return a->m_uuid == enemy->GetUniqueID(); });
    if (it != m_enemies->m_pool.end()) {
        (*it)->remove();
    }
}

void AGameManager::RemoveBullet(AActor *bullet) {
    vector<shared_ptr<Actor>>::iterator it = find_if(m_bullets->m_pool.begin(), m_bullets->m_pool.end(),
        [&](shared_ptr<Actor> a) { return a->m_uuid == bullet->GetUniqueID(); });
    if (it != m_bullets->m_pool.end()) {
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

void AGameManager::addBoss(const FVector2D &position, float direction, shared_ptr<EnemyType> type) {
}

void AGameManager::shipDestroyed() {
    clearBullets();

    m_shipsRemaining--;
    if (m_shipsRemaining < 0) {
        startGameOver();
    }
}

void AGameManager::clearBullets() {
    for (int i = 0; i < m_bullets->m_pool.size(); i++) {
        if (!m_bullets->m_pool[i]->m_isAlive) {
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

    //m_shots->clear();
    
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
    //m_stageManager->tick();
}

void AGameManager::inGameTick() {
#if 0
    long nowTick = (long)UGameplayStatics::GetRealTimeSeconds(GetWorld()) * 1000.0;
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
#else
    stageTick();
#endif

    m_field->tick();
    m_ship->tick();
    //m_shots->tick();
    m_enemies->tick();
    BulletActor::resetTotalBulletsSpeed();
    m_bullets->tick();

    // set pause

    if (BulletActor::m_totalBulletsSpeed > SLOWDOWN_START_BULLETS_SPEED[m_mode]) {
        float sm = BulletActor::m_totalBulletsSpeed / SLOWDOWN_START_BULLETS_SPEED[m_mode];
        if (sm > 1.75) {
            sm = 1.75;
        }
        m_interval += (sm * INTERVAL_BASE - m_interval) * 0.01;
    } else {
        m_interval += (INTERVAL_BASE - m_interval) * 0.08;
    }
}

void AGameManager::titleTick() {
}

void AGameManager::gameOverTick() {
}

void AGameManager::pauseTick() {
}