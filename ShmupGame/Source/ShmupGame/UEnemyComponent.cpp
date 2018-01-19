// Fill out your copyright notice in the Description page of Project Settings.

#include "UEnemyComponent.h"

#include "BulletManager.h"
#include "Bullet.h"
#include "UBulletComponent.h"
#include "UPlayerComponent.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"

using namespace std;

UEnemyComponent::UEnemyComponent() :
    m_owner(GetOwner()),
    m_bulletManager(new BulletManager(this)) {
    //m_bulletComponent(CreateDefaultSubobject<UBulletComponent>(TEXT("UBulletComponent"))) {
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

UEnemyComponent::~UEnemyComponent() {
}

void UEnemyComponent::BeginPlay() {
    Super::BeginPlay();

    FVector location = m_owner->GetActorLocation();
    // @TODO: temporary fix
    //m_bulletComponent->setX(location.X);
    //m_bulletComponent->setY(location.Z);

    /**
     * @TODO load/parse all relevant enemy xml files elsewhere
     * configure relevant xml files for this particular enemy instance
     * via the editor as exposed public property
     */
    FString sourcePath = FPaths::GameSourceDir();
    string srcPath = TCHAR_TO_UTF8(*sourcePath);

    //m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/move.xml"));
    //m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/basic.xml"));
    m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/10way.xml"));
    //m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/round_1_boss.xml"));
    //m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/hibachi_4.xml"));
    //m_bulletParsers.push_back(new BulletMLParserTinyXML(srcPath + "xml/kitiku_3.xml"));

    APawn *player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
    if (player != nullptr) {
        m_playerComponent = player->FindComponentByClass<UPlayerComponent>();
    }

    for (size_t i = 0; i < m_bulletParsers.size(); ++i) {
        m_bulletParsers[i]->build();
        // @TODO: replace target with player ship
        m_bulletManager->createBullet(m_bulletParsers[i],
            new Bullet(location.X, location.Z, 0, 0),
            //spawnBulletActor(location.X, location.Z, 0, 0),
            m_playerComponent);
    }
}

void UEnemyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *TickFunction) {
    Super::TickComponent(DeltaTime, TickType, TickFunction);

    m_bulletManager->tick();
}

Bullet *UEnemyComponent::spawnBulletActor(float x, float y, float direction, float speed) {
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
