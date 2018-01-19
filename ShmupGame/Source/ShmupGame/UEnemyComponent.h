// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "bulletml/bulletmlparser.h"

#include "UEnemyComponent.generated.h"

class BulletManager;
class Bullet;
class UBulletComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHMUPGAME_API UEnemyComponent : public UActorComponent {
	GENERATED_BODY()

public:
    UEnemyComponent();
    ~UEnemyComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *TickFunction) override;	

    Bullet *spawnBulletActor(float x, float y, float direction, float speed);

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile")
    UClass *bp_projectileType;

    //UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Target")
    //UObject *bp_target;
    
private:
    AActor *m_owner;
    BulletManager *m_bulletManager;
    // @TODO: replace this with a common class both [Enemy|Bullet]Components can inherit from -- and use as targets; tickers
    //UBulletComponent *m_bulletComponent;
    Bullet *m_playerComponent;
    std::vector<BulletMLParser *> m_bulletParsers;
};