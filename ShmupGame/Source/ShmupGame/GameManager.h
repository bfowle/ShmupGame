#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "ActorPool.h"
#include "EnemyType.h"
#include "Random.h"

#include <array>
#include <memory>
#include <vector>

#include "GameManager.generated.h"

class StageManager;
class BarrageManager;
class Enemy;
class Field;
class Ship;
class BulletActorPool;
class BulletMLParser;

UCLASS()
class SHMUPGAME_API AGameManager : public AGameMode/*, public TSharedFromThis<AGameManager>*/ {
    GENERATED_BODY()

public:
    AGameManager();

    //TSharedRef<AGameManager> getPtr();

    void InitGame(const FString &MapName, const FString &Options, FString &ErrorMessage);
    void StartPlay();
    void Tick(float DeltaSeconds);
    //InitGameState
    //InitNewPlayer
    //IsPaused | SetPause
    //RestartPlayer
    //SetPlayerDefaults
    //StartToLeaveMap

    void tick();
    void close();
    void startStage(int difficulty, int parsecSlot, int startParsec, int mode);
    void addEnemy(const FVector2D &position, float direction, std::shared_ptr<EnemyType> type, BulletMLParser *moveParser);
    void addBoss(const FVector2D &position, float direction, std::shared_ptr<EnemyType> type);
    //void addShot(const FVector2D &position, float direction);
    void addRoll();
    void addLock();
    void releaseRoll();
    void releaseLock();
    void shipDestroyed();
    void clearBullets();

private:
    void initShipState();
    void startInGame();
    void startTitle();
    void startGameOver();
    void startPause();
    void resumePause();
    void stageTick();
    void inGameTick();
    void titleTick();
    void gameOverTick();
    void pauseTick();

public:
    enum {
        TITLE,
        IN_GAME,
        GAME_OVER,
        PAUSE
    };

    enum {
        PRACTICE,
        NORMAL,
        HARD,
        EXTREME,
        QUIT
    };

    enum {
        ROLL,
        LOCK
    };

    const int INTERVAL_BASE = 16;

    UWorld *m_world;
    UClass *bp_enemyClass;
    UClass *bp_bulletClass;

    int m_mode, m_state;
    int m_difficulty, m_parsecSlot;
    float m_deltaSeconds;

private:
    enum {
        ENEMY_MAX = 32,
        FIRST_EXTEND = 200000,
        EVERY_EXTEND = 500000,
        LEFT_MAX = 4,
        BOSS_WING_NUM = 4
    };

    Random m_random;
    std::shared_ptr<Field> m_field;
    std::shared_ptr<Ship> m_ship;
    std::shared_ptr<ActorPool> m_enemies;
    std::shared_ptr<BulletActorPool> m_bullets;
    std::shared_ptr<ActorPool> m_shots;
    std::shared_ptr<ActorPool> m_rolls;
    std::shared_ptr<ActorPool> m_locks;
    std::shared_ptr<StageManager> m_stageManager;
    std::shared_ptr<BarrageManager> m_barrageManager;

    static const int SLOWDOWN_START_BULLETS_SPEED[2];

    int m_left;
    int m_score, m_extendScore;
    int m_cnt;
    int m_pauseCnt;
    int m_bossShield;
    std::array<int, BOSS_WING_NUM> m_bossWingShield;

    float m_interval;
    int m_frame, m_maxSkipFrame;
    long m_previousTick;
};