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

    UBulletComponent *spawnBulletActor(float x, float y, float direction, float speed);

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Projectile Settings")
    UClass *bp_projectileType;
    
private:
    AActor *m_owner;
    BulletManager *m_bulletManager;
    // @TODO: replace this with a common class both [Enemy|Bullet]Components can inherit from -- and use as targets; tickers
    UBulletComponent *m_bulletComponent;
    std::vector<BulletMLParser *> m_bulletParsers;
};