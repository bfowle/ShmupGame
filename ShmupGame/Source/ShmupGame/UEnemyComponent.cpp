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
    APawn *player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (player != nullptr) {
        m_playerComponent = player->FindComponentByClass<UPlayerComponent>();

        for (size_t i = 0; i < m_bulletParsers.size(); ++i) {
            // @TODO: sequence of firing bullets, not just all at once
            m_bulletManager->createBullet(m_bulletParsers[i], this, m_playerComponent);
        }
    }
    */
}

void UEnemyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *TickFunction) {
    Super::TickComponent(DeltaTime, TickType, TickFunction);

    //m_bulletManager->tick();

    tick();
    m_owner->SetActorLocation(FVector(m_x, 100.0, m_y));

    //UE_LOG(LogTemp, Warning, TEXT(" TICK: %s [%f, %f] (d: %f, s: %f) ... "), *GetName(), m_x, m_y, m_direction, m_speed);
}
