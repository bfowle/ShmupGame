// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Bullet.h"

#include "ABulletActor.generated.h"

UCLASS(/*ClassGroup = (Custom), meta = (BlueprintSpawnableComponent)*/)
class SHMUPGAME_API ABulletActor : public AActor, public Bullet {
    GENERATED_BODY()

public:
    ABulletActor();
    ~ABulletActor();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
};