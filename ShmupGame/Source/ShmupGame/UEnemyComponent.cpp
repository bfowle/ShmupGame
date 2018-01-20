// Fill out your copyright notice in the Description page of Project Settings.

#include "UEnemyComponent.h"

#include "BulletManager.h"
#include "Movable.h"
#include "UBulletComponent.h"
#include "UPlayerComponent.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"

UEnemyComponent::UEnemyComponent() {
    //m_bulletComponent(CreateDefaultSubobject<UBulletComponent>(TEXT("UBulletComponent"))) {
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

UEnemyComponent::~UEnemyComponent() {
}

void UEnemyComponent::BeginPlay() {
    Super::BeginPlay();

    m_x = m_owner->GetActorLocation().X;
    m_y = m_owner->GetActorLocation().Z;

    /*
    FString sourcePath = FPaths::GameSourceDir();
    string srcPath = TCHAR_TO_UTF8(*sourcePath);

    //m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/move.xml"));
    //m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/basic.xml"));
    //m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/10way.xml"));
    m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/all_around.xml"));
    //m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/homing_laser.xml"));
    //m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/hibachi_4.xml"));
    //m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/kitiku_3.xml"));
    //m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/round_1_boss.xml"));
    //m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/round_5_boss_gara_3.xml"));
    */

    APawn *player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (player != nullptr) {
        m_playerComponent = player->FindComponentByClass<UPlayerComponent>();
    }

    for (size_t i = 0; i < m_bulletParsers.size(); ++i) {
        // @TODO: sequence of firing bullets, not just all at once
        m_bulletManager->createBullet(m_bulletParsers[i],
            new Movable(m_owner->GetActorLocation().X, m_owner->GetActorLocation().Z, 0, 0),
            m_playerComponent);
    }
}

void UEnemyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *TickFunction) {
    Super::TickComponent(DeltaTime, TickType, TickFunction);

    m_bulletManager->tick();

    //m_owner->SetActorLocation(FVector(m_x, 100.0, m_y));

    //UE_LOG(LogTemp, Warning, TEXT(" TICK: %s [%f, %f] (d: %f, s: %f) ... "), *GetName(), m_x, m_y, m_direction, m_speed);
}
