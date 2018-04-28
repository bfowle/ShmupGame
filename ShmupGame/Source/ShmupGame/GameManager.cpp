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
#include "Misc/Paths.h"

#include "bulletml/bulletml.h"
#include "bulletml/bulletmlparser-tinyxml.h"

using namespace std;

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

    //m_stageManager.reset(new StageManager());
    //m_stageManager->init(m_barrageManager, this);
}

void AGameManager::StartPlay() {
    Super::StartPlay();

    TWeakObjectPtr<APawn> player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (player.IsValid()) {
        m_ship->setPlayerPawn(player);
    }

    //startTitle();
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
}

void AGameManager::AddShot(const FVector &position, float direction) {
    UE_LOG(LogTemp, Warning, TEXT("FIRING SHOT: %s | %f"), *position.ToString(), direction);
}

void AGameManager::AddEnemy(AActor *actor, FString moveFilePath) {
    shared_ptr<Enemy> enemy = static_pointer_cast<Enemy>(m_enemies->getInstance());
    if (!enemy) {
        return;
    }

    //StageManager::EnemySquadron *squadron = &m_stageManager->m_squadrons[0];

    // @TODO: move this into a helper class
    int32 found = moveFilePath.Find("BulletML");
    if (found > -1) {
        moveFilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()) +
            moveFilePath.RightChop(found);
    }

    vector<BulletMLParserTinyXML *>::const_iterator it = find_if(m_barrageManager->m_parser.cbegin(), m_barrageManager->m_parser.cend(),
        [&](BulletMLParserTinyXML *a) { return a->getName() == const_cast<char *>(TCHAR_TO_UTF8(*moveFilePath)); });
    if (it != m_barrageManager->m_parser.cend()) {
        enemy->set(FVector2D(actor->GetActorLocation().X, actor->GetActorLocation().Z), M_PI/*, squadron->m_type*/, (*it));
        enemy->setActor(actor);
    }
}

void AGameManager::RemoveEnemy(AActor *enemy) {
    vector<shared_ptr<Actor>>::const_iterator it = find_if(m_enemies->m_pool.cbegin(), m_enemies->m_pool.cend(),
        [&](shared_ptr<Actor> a) { return a->m_uuid == enemy->GetUniqueID(); });
    if (it != m_enemies->m_pool.cend()) {
        (*it)->remove();
    }
}

void AGameManager::RemoveBullet(AActor *bullet) {
    vector<shared_ptr<Actor>>::const_iterator it = find_if(m_bullets->m_pool.cbegin(), m_bullets->m_pool.cend(),
        [&](shared_ptr<Actor> a) { return a->m_uuid == bullet->GetUniqueID(); });
    if (it != m_bullets->m_pool.cend()) {
        (*it)->remove();
    }
}

void AGameManager::close() {
    m_barrageManager->unloadBulletMLFiles();
}

void AGameManager::startStage(int difficulty) {
    m_enemies->clear();
    m_bullets->clear();

    m_difficulty = difficulty;

    //int stageType = m_random.nextInt(99999);

    switch (difficulty) {
    case PRACTICE:
        //m_stageManager->setRank(1, 4, stageType);
        m_ship->setSpeedRate(0.7);
        break;
    case NORMAL:
        //m_stageManager->setRank(10, 8, stageType);
        m_ship->setSpeedRate(0.9);
        break;
    case HARD:
        //m_stageManager->setRank(22, 12, stageType);
        m_ship->setSpeedRate(1);
        break;
    case EXTREME:
        //m_stageManager->setRank(36, 16, stageType);
        m_ship->setSpeedRate(1.2);
        break;
    case QUIT:
        //m_stageManager->setRank(0, 0, 0);
        m_ship->setSpeedRate(1);
        break;
    }
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
    }
}

void AGameManager::initShipState() {
    m_shipsRemaining = 2;
    m_score = 0;

    m_ship->start();
}

void AGameManager::startInGame() {
    m_state = IN_GAME;

    initShipState();

    startStage(m_difficulty);
}

void AGameManager::startTitle() {
    m_state = TITLE;
    //m_title->start();

    initShipState();
    m_bullets->clear();
    m_ship->m_cnt = 0;

    startStage(m_difficulty);
}

void AGameManager::startGameOver() {
    m_state = GAME_OVER;

    //m_shots->clear();
}

void AGameManager::startPause() {
    m_state = PAUSE;
}

void AGameManager::resumePause() {
    m_state = IN_GAME;
}

void AGameManager::inGameTick() {
    m_field->tick();
    m_ship->tick();
    //m_shots->tick();
    m_enemies->tick();

    //UE_LOG(LogTemp, Warning, TEXT(" **************** IN GAME TICK: START *********************** "));
    BulletActor::resetTotalBulletsSpeed();
    m_bullets->tick();
    //UE_LOG(LogTemp, Warning, TEXT(" **************** IN GAME TICK: END *********************** "));

    // set pause
}

void AGameManager::titleTick() {
}

void AGameManager::gameOverTick() {
}

void AGameManager::pauseTick() {
}