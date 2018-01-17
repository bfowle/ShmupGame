// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "UMoverComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SHMUPGAME_API UMoverComponent : public UActorComponent {
    GENERATED_BODY()

public:
    UMoverComponent();
    ~UMoverComponent();

protected:
    UMoverComponent(double x, double y, double d, double s);

protected:
    virtual void BeginPlay() override;

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *TickFunction) override;
};