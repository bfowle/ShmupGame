// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Movable.h"

#include "bulletml/bulletmlparser.h"

#include <vector>

#include "UMovableComponentBase.generated.h"

class BulletManager;
class UBulletComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHMUPGAME_API UMovableComponentBase : public UActorComponent, public Movable {
	GENERATED_BODY()

public:
	UMovableComponentBase();

protected:
	virtual void BeginPlay() override;
    virtual void OnComponentDestroyed(bool bDestroyingHierarchy);

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

    UBulletComponent *spawnBulletActor(float x, float y, float direction, float speed);

public:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Projectile", meta=(FilePathFilter="*xml"))
    TArray<FFilePath> m_bulletFiles;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Projectile")
    UClass *bp_projectileType;

protected:
    AActor *m_owner;
    //BulletManager *m_bulletManager;    
    std::vector<BulletMLParser *> m_bulletParsers;
    std::vector<AActor *> m_spawnedActors;
};
