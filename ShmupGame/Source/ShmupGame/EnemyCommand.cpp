// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCommand.h"
//#include "EnemyBullet.h"

UEnemyCommand::UEnemyCommand() {
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;

    //enemyBullet = new EnemyBullet();
}

UEnemyCommand::~UEnemyCommand() {
}

void UEnemyCommand::BeginPlay() {
    Super::BeginPlay();
}

void UEnemyCommand::TickComponent(float deltaTime, ELevelTick tickType, FActorComponentTickFunction *tickFunction) {
    Super::TickComponent(deltaTime, tickType, tickFunction);
}