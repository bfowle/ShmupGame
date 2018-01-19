// Fill out your copyright notice in the Description page of Project Settings.

#include "UEnemyComponent.h"

#include "BulletManager.h"
#include "Bullet.h"
#include "EnemyCommand.h"

#include "bulletml/bulletmlparser.h"
#include "bulletml/bulletmlparser-tinyxml.h"

using namespace std;

UEnemyComponent::UEnemyComponent() :
    m_owner(GetOwner()),
    m_bulletManager(new BulletManager(this)),
    m_bulletActor(CreateDefaultSubobject<ABulletActor>(TEXT("ABulletActor"))) {
    // Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
    // off to improve performance if you don't need them.
    PrimaryComponentTick.bCanEverTick = true;
}

UEnemyComponent::~UEnemyComponent() {
}

void UEnemyComponent::BeginPlay() {
    Super::BeginPlay();

    FVector location = m_owner->GetActorLocation();

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

    for (size_t i = 0; i < m_bulletParsers.size(); ++i) {
        m_bulletParsers[i]->build();
        // @TODO: replace target with player ship
        m_bulletManager->createBullet(m_bulletParsers[i], spawnBulletActor(location.X, location.Z, 0, 0), m_bulletActor);
    }
}

void UEnemyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *TickFunction) {
    Super::TickComponent(DeltaTime, TickType, TickFunction);

    m_bulletManager->tick();
}

ABulletActor *UEnemyComponent::spawnBulletActor(float x, float y, float direction, float speed) {
    ABulletActor *actor = GetWorld()->SpawnActor<ABulletActor>(ABulletActor::StaticClass(),
        FVector::ZeroVector, FRotator::ZeroRotator);
    actor->SetOwner(m_owner);
    //actor->setProjectileType(bp_projectileType);
    actor->setX(x);
    actor->setY(y);
    actor->setDirection(direction);
    actor->setSpeed(speed);
    return actor;
}
