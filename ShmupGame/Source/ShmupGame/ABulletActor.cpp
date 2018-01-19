// Fill out your copyright notice in the Description page of Project Settings.

#include "ABulletActor.h"

ABulletActor::ABulletActor() {
    //m_staticMesh(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletActorMesh"))) {
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryActorTick.bCanEverTick = true;

    //m_staticMesh->SetupAttachment(RootComponent);
    //m_staticMesh->SetStaticMesh(m_bpStaticMesh);
    
}

ABulletActor::~ABulletActor() {
}

void ABulletActor::BeginPlay() {
    Super::BeginPlay();
}

void ABulletActor::Tick(float DeltaTime) {
    Super::Tick(DeltaTime);

    //m_staticMesh->SetRelativeLocation(FVector(m_x, m_staticMesh->RelativeLocation.Y, m_y));

    //UE_LOG(LogTemp, Warning, TEXT(" [%s] ABulletActor::TickComponent() (%f, %f) | [%f, %f] "), *GetName(),
    //    m_x, m_y, m_direction, m_speed);
}

/*
void ABulletActor::setProjectileType(UClass *projectileType) {
    UObject *projectile = NewObject<UObject>(this, projectileType);
    UE_LOG(LogTemp, Warning, TEXT(" -- set projectile type -- %s [%s]"), *projectile->GetName(), *projectileType->GetName());
    //projectile->SetOwner(GetOwner());
}
*/