// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyBullet.h"
#include "EnemyCommand.generated.h"

class EnemyBullet;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHMUPGAME_API UEnemyCommand : public UActorComponent {
	GENERATED_BODY()

public:
    UEnemyCommand();
    ~UEnemyCommand();

    EnemyBullet *enemyBullet;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *TickFunction) override;	
};