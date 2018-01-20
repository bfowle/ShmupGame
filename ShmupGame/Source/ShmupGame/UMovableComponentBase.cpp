// Fill out your copyright notice in the Description page of Project Settings.

#include "UMovableComponentBase.h"

#include "BulletManager.h"
#include "Movable.h"
#include "UBulletComponent.h"
#include "UPlayerComponent.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"

using namespace std;

UMovableComponentBase::UMovableComponentBase() :
    m_owner(GetOwner()),
    m_bulletManager(new BulletManager(this)) {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UMovableComponentBase::BeginPlay() {
	Super::BeginPlay();

    FString sourcePath = FPaths::GameSourceDir();

    // @TODO: is this the proper place to parse these...?
    for (auto &file : m_bulletFiles) {
        file.FilePath.RemoveFromStart(sourcePath);
        if (FPaths::FileExists(sourcePath + file.FilePath)) {
            m_bulletParsers.push_back(new BulletMLParserTinyXML(TCHAR_TO_UTF8(*(sourcePath + file.FilePath))));
        }
    }

    for (size_t i = 0; i < m_bulletParsers.size(); ++i) {
        m_bulletParsers[i]->build();
    }
}

void UMovableComponentBase::OnComponentDestroyed(bool bDestroyingHierarchy) {
    for (AActor *actor : m_spawnedActors) {
        actor->Destroy();
    }

    Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UMovableComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // @NOTE: each class that inherits from this should handle its own TickComponent
}

Movable *UMovableComponentBase::spawnBulletActor(float x, float y, float direction, float speed) {
    AActor *actor = GetWorld()->SpawnActor<AActor>(bp_projectileType,
        FVector::ZeroVector, FRotator::ZeroRotator);
    actor->AttachToActor(m_owner, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("Bullets"));
    actor->SetOwner(m_owner);
    m_spawnedActors.push_back(actor);

    UBulletComponent *bullet = actor->FindComponentByClass<UBulletComponent>();
    bullet->setX(x);
    bullet->setY(y);
    bullet->setDirection(direction);
    bullet->setSpeed(speed);

    return bullet;
}
