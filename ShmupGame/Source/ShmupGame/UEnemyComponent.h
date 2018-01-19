// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "bulletml/bulletmlparser.h"

#include "UEnemyComponent.generated.h"

class BulletManager;
class Bullet;
class ABulletActor;

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

    ABulletActor *spawnBulletActor(float x, float y, float direction, float speed);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UObject *bp_projectileType;

private:
    BulletManager *m_bulletManager;
    ABulletActor *m_bulletActor;
    std::vector<BulletMLParser *> m_bulletParsers;
};