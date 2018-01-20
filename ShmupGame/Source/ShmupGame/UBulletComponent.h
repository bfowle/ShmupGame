// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Movable.h"

#include "UBulletComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHMUPGAME_API UBulletComponent : public UActorComponent, public Movable {
    GENERATED_BODY()

public:
    UBulletComponent();
    ~UBulletComponent();

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *TickFunction) override;

private:
    UStaticMeshComponent *m_staticMesh;
};