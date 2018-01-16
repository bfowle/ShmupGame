// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyCommand.h"
#include "Bullet.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"

UEnemyCommand::UEnemyCommand() {
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

UEnemyCommand::~UEnemyCommand() {
}

void UEnemyCommand::BeginPlay() {
    Super::BeginPlay();

    FString sourcePath = FPaths::GameSourceDir();
    FString filePath = sourcePath + "Bullets/test.xml";
    //UE_LOG(LogTemp, Warning, TEXT("%s"), *filePath);

    BulletMLParser *bp = new BulletMLParserTinyXML(TCHAR_TO_UTF8(*filePath));
    bp->build();

    Bullet *b = new Bullet();
    enemyBullet = new EnemyBullet(bp, b);
}

void UEnemyCommand::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *TickFunction) {
    Super::TickComponent(DeltaTime, TickType, TickFunction);

    if (enemyBullet) {
        enemyBullet->run();
    }
}