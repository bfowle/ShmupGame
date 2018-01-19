// Fill out your copyright notice in the Description page of Project Settings.

#include "UPlayerComponent.h"

// Sets default values for this component's properties
UPlayerComponent::UPlayerComponent() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

UPlayerComponent::~UPlayerComponent() {
}

void UPlayerComponent::BeginPlay() {
	Super::BeginPlay();
}

void UPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    m_x = GetOwner()->GetActorLocation().X;
    m_y = GetOwner()->GetActorLocation().Z;
}