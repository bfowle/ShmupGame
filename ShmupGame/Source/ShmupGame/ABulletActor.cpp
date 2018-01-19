// Fill out your copyright notice in the Description page of Project Settings.

#include "ABulletActor.h"

ABulletActor::ABulletActor() {
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryActorTick.bCanEverTick = true;
}

ABulletActor::~ABulletActor() {
}

void ABulletActor::BeginPlay() {
    Super::BeginPlay();
}

void ABulletActor::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    UE_LOG(LogTemp, Warning, TEXT(" [%s] ABulletActor::TickComponent() (%f, %f) | [%f, %f] "), *GetName(),
        m_x, m_y, m_direction, m_speed);
}