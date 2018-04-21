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
    void AddEnemy(AActor *enemy, FString moveFilePath);
    UFUNCTION(BlueprintCallable, Category = GameManager)
    void RemoveEnemy(AActor *enemy);

    UFUNCTION(BlueprintImplementableEvent, Category = GameManager)
    AActor *AddBullet(FVector Position);
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
    void startStage(int difficulty);
    void shipDestroyed();
    void clearBullets();

private:
    void initShipState();
    void startInGame();
    void startTitle();
    void startGameOver();
    void startPause();
    void resumePause();
    void inGameTick();
    void titleTick();
    void gameOverTick();
    void pauseTick();

public:
    enum { TITLE, IN_GAME, GAME_OVER, PAUSE };
    enum { PRACTICE, NORMAL, HARD, EXTREME, QUIT };

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameManager)
    TSubclassOf<AActor> BP_BulletClass;

    TWeakObjectPtr<UWorld> m_world;

    int m_state;
    int m_difficulty;
    float m_deltaSeconds;

private:
    enum {
        BULLET_MAX = 1024 << 8,
        ENEMY_MAX = 32
    };

    Random m_random;
    TWeakObjectPtr<APawn> m_player;

    std::shared_ptr<Field> m_field;
    std::shared_ptr<Ship> m_ship;
    std::shared_ptr<ActorPool> m_enemies;
    std::shared_ptr<BulletActorPool> m_bullets;
    //std::shared_ptr<ActorPool> m_shots;
    std::shared_ptr<StageManager> m_stageManager;
    std::shared_ptr<BarrageManager> m_barrageManager;

    int m_score;
    int m_shipsRemaining;
};