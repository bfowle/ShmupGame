#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "GameFramework/GameModeBase.h"

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
class SHMUPGAME_API AGameManager : public AGameModeBase {
    GENERATED_BODY()

public:
    AGameManager();

    virtual void InitGame(const FString &MapName, const FString &Options, FString &ErrorMessage) override;
    virtual void StartPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    UFUNCTION(BlueprintCallable, Category = GameManager)
    void AddShot(const FVector &position, float direction);

    UFUNCTION(BlueprintCallable, Category = GameManager)
    void RemoveEnemy(AActor *enemy);

    UFUNCTION(BlueprintCallable, Category = GameManager)
    void RemoveBullet(AActor *bullet);

    //GameModeBase:
    //  ReplicatedWorldTimeSeconds |
    //  ServerWorldTimeSecondsDelta |
    //  ServerWorldTimeSecondsUpdateFrequency |
    //  TimerHandle_UpdateServerTimeSeconds() |
    //  GetServerWorldTimeSeconds() |
    //  UpdateServerTimeSeconds()
    //GameMode:
    //  ElapsedTime

    //InitGameState
    //HandleBeginPlay | HasBegunPlay | HasMatchStarted

    //IsPaused | SetPause

    //AddPlayerState | RemovePlayerState
    //SetPlayerDefaults
    //RestartPlayer

    //StartToLeaveMap

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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameManager)
    TSubclassOf<AActor> BP_EnemyClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameManager)
    TSubclassOf<AActor> BP_BulletClass;

    TWeakObjectPtr<UWorld> m_world;

    int m_mode, m_state;
    int m_difficulty, m_parsecSlot;
    float m_deltaSeconds;

private:
    enum {
        BULLET_MAX = 512,
        ENEMY_MAX = 32,
        ROLL_MAX = 4,
        LOCK_MAX = 4,
        FIRST_EXTEND = 200000,
        EVERY_EXTEND = 500000,
        LEFT_MAX = 4,
        BOSS_WING_TOTAL = 4
    };

    static const int SLOWDOWN_START_BULLETS_SPEED[2];

    Random m_random;
    std::shared_ptr<Field> m_field;
    std::shared_ptr<Ship> m_ship;
    std::shared_ptr<ActorPool> m_enemies;
    std::shared_ptr<BulletActorPool> m_bullets;
    //std::shared_ptr<ActorPool> m_shots;
    std::shared_ptr<ActorPool> m_rolls;
    std::shared_ptr<ActorPool> m_locks;
    std::shared_ptr<StageManager> m_stageManager;
    std::shared_ptr<BarrageManager> m_barrageManager;

    int m_cnt;
    int m_score, m_extendScore;
    int m_shipsRemaining;
    int m_pauseCnt;
    int m_bossShield;
    std::array<int, BOSS_WING_TOTAL> m_bossWingShield;

    int m_interval;
    int m_frame, m_maxSkipFrame;
    long m_previousTick;
};