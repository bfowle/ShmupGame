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
class ShmupBullet;
class BulletActorPool;
class BulletMLParser;
class UCameraComponent;
class UInstancedStaticMeshComponent;

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

    UFUNCTION(BlueprintCallable, Category = GameManager)
    FVector ClampToScreen(FVector position);

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

    int32 addBullet(FVector position);
    FVector updateBullet(BulletActor *actor, std::shared_ptr<ShmupBullet> bullet, float speedRank);

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

    bool shouldRemoveInstance(FVector position);

    inline FVector2D calculateScreenBounds(float fieldOfView, float aspectRatio, float depth) {
        return FVector2D(abs((tan((((M_PI / 180.0) * fieldOfView) / 2.0))) * depth),
            abs((tan((((M_PI / 180.0) * (fieldOfView / aspectRatio)) / 2.0))) * depth));
    }

public:
    enum { TITLE, IN_GAME, GAME_OVER, PAUSE };
    enum { PRACTICE, NORMAL, HARD, EXTREME, QUIT };

    TWeakObjectPtr<UWorld> m_world;
    TWeakObjectPtr<AActor> m_cameraActor;
    TWeakObjectPtr<UCameraComponent> m_cameraComponent;
    TWeakObjectPtr<UInstancedStaticMeshComponent> m_enemyBulletISM;

    int m_state;
    int m_difficulty;
    float m_deltaSeconds;

private:
    enum {
        BULLET_MAX = 1024, // << 8,
        ENEMY_MAX = 32
    };

    Random m_random;
    //TWeakObjectPtr<APawn> m_player;

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