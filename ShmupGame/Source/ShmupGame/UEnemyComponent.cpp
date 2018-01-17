// Fill out your copyright notice in the Description page of Project Settings.

#include "UEnemyComponent.h"

#include "BulletManager.h"
#include "Bullet.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"

UEnemyComponent::UEnemyComponent() :
    m_bulletManager(new BulletManager()),
    m_moverComponent(CreateDefaultSubobject<UMoverComponent>(TEXT("MoverComponent"))) {
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

UEnemyComponent::~UEnemyComponent() {
}

void UEnemyComponent::BeginPlay() {
    Super::BeginPlay();

    /**
     * @TODO load/parse all relevant enemy xml files elsewhere
     * configure relevant xml files for this particular enemy instance
     * via the editor as exposed public property
     */
    FString sourcePath = FPaths::GameSourceDir();
    FString filePath = sourcePath + "Bullets/test.xml";
    UE_LOG(LogTemp, Warning, TEXT("%s"), *filePath);

    BulletMLParser *parser = new BulletMLParserTinyXML(TCHAR_TO_UTF8(*filePath));
    parser->build();

    Bullet *bullet = new Bullet();
    m_bulletManager->createBullet(parser, bullet, m_moverComponent);
}

void UEnemyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *TickFunction) {
    Super::TickComponent(DeltaTime, TickType, TickFunction);

    m_bulletManager->tick();
}