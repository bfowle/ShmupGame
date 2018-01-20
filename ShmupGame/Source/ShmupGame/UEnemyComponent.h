// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "UMovableComponentBase.h"

#include "UEnemyComponent.generated.h"

class Movable;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHMUPGAME_API UEnemyComponent : public UMovableComponentBase {
	GENERATED_BODY()

public:
    UEnemyComponent();
    ~UEnemyComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *TickFunction) override;	

private:
    Movable *m_playerComponent;
};