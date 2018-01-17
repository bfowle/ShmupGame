// Fill out your copyright notice in the Description page of Project Settings.

#include "UMoverComponent.h"

UMoverComponent::UMoverComponent() {
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

UMoverComponent::UMoverComponent(double x, double y, double d, double s) {
}

UMoverComponent::~UMoverComponent() {
}

void UMoverComponent::BeginPlay() {
    Super::BeginPlay();
}

void UMoverComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *TickFunction) {
    Super::TickComponent(DeltaTime, TickType, TickFunction);
}