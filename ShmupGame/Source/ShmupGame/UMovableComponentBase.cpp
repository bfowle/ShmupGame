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

UMovableComponentBase::UMovableComponentBase() :
    m_owner(GetOwner()),
    m_bulletManager(new BulletManager(this)) {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void UMovableComponentBase::BeginPlay() {
	Super::BeginPlay();
}

void UMovableComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

Movable *UMovableComponentBase::spawnBulletActor(float x, float y, float direction, float speed) {
    AActor *actor = GetWorld()->SpawnActor<AActor>(bp_projectileType,
        FVector::ZeroVector, FRotator::ZeroRotator);
    actor->AttachToActor(m_owner, FAttachmentTransformRules::KeepRelativeTransform, TEXT("Bullets"));
    actor->SetOwner(m_owner);

    UBulletComponent *bullet = actor->FindComponentByClass<UBulletComponent>();
    bullet->setX(x);
    bullet->setY(y);
    bullet->setDirection(direction);
    bullet->setSpeed(speed);

    return bullet;
}
