// Fill out your copyright notice in the Description page of Project Settings.

#include "UBulletComponent.h"

UBulletComponent::UBulletComponent() :
    m_owner(GetOwner()) {
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

UBulletComponent::~UBulletComponent() {
}

void UBulletComponent::BeginPlay() {
    Super::BeginPlay();
}

void UBulletComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *TickFunction) {
    Super::TickComponent(DeltaTime, TickType, TickFunction);

    m_owner->SetActorLocation(FVector(m_x, 100.0, m_y));

    //UE_LOG(LogTemp, Warning, TEXT(" [%s] UBulletComponent::TickComponent() (%f, %f) | [%f, %f] "), *GetName(),
    //    m_x, m_y, m_direction, m_speed);
}
