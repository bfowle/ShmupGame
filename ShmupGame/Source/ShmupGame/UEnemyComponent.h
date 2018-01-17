// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "UEnemyComponent.generated.h"

class BulletManager;
class UMoverComponent;

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

private:
    BulletManager *m_bulletManager;
    UMoverComponent *m_moverComponent;
};