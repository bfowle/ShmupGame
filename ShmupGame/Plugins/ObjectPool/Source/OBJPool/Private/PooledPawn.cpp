/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PooledPawn.h"
#include "OBJPoolShared.h"
#include "Runtime/Engine/Classes/Particles/ParticleSystemComponent.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

APooledPawn::APooledPawn(const FObjectInitializer &OBJ) : Super(OBJ) {
	LifeSpanPool = 0.f;
}

void APooledPawn::Initialize() {
	const auto &Settings = GetMutableDefault<UPoolSettings>();
	//
	Spawned = false;
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	SetLifeSpan(0.f);
	//
	TInlineComponentArray<UActorComponent*> Components;
	GetComponents(Components);
	//
	for (const auto &C : Components) {
		const auto &P = Cast<UPrimitiveComponent>(C);
		if (P) {
			P->SetSimulatePhysics(false);
			P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			P->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			P->SetPhysicsLinearVelocity(FVector::ZeroVector);
			P->SetComponentTickEnabled(false);
			P->SetVisibility(false,true);
	} C->Deactivate();}
	//
	if (OwningPool && OwningPool->IsValidLowLevelFast()) {
		OwningPool->ReturnActor(const_cast<APooledPawn*>(this));
	}
}

void APooledPawn::BeginPlay() {
	if (LifeSpanPool>0.0001f) {
		const FTimerDelegate Timer = FTimerDelegate::CreateUObject(this,&APooledPawn::ReturnToPool);
		GetWorld()->GetTimerManager().SetTimer(LifeSpanHandle,Timer,LifeSpanPool,false);
	} Super::BeginPlay();
}

void APooledPawn::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	OnPoolBeginPlay.Clear();
	OnPoolEndPlay.Clear();
	//
	Super::EndPlay(EndPlayReason);
}

void APooledPawn::SpawnFromPool(const bool Reconstruct, const FPoolSpawnOptions &SpawnOptions, const FTransform &SpawnTransform) {
	if (!OwningPool||!OwningPool->IsValidLowLevelFast()) {
		UE_LOG(LogTemp,Warning,TEXT("{Pool}:: %s"),TEXT("Actor trying to spawn from Pool, but Owning Pool Component is invalid!"));
	return;} Spawned = true;
	//
	const auto &Settings = GetMutableDefault<UPoolSettings>();
	//
	TInlineComponentArray<UActorComponent*> Components;
	GetComponents(Components);
	//
	for (const auto &C : Components) {
		const auto &P = Cast<UPrimitiveComponent>(C);
		if (P) {
			P->SetCollisionEnabled((TEnumAsByte<ECollisionEnabled::Type>)(uint8)SpawnOptions.CollisionType);
			P->SetComponentTickEnabled(SpawnOptions.ActorTickEnabled);
			P->SetSimulatePhysics(SpawnOptions.SimulatePhysics);
			P->SetVisibility(true,true);
	} C->Activate(true);}
	//
	if (Settings->ReinitializeInstances) {
		SetActorLocationAndRotation(SpawnTransform.GetLocation(),SpawnTransform.GetRotation(),false,nullptr,ETeleportType::TeleportPhysics);
		SetActorScale3D(SpawnTransform.GetScale3D());
		SetActorTickEnabled(SpawnOptions.ActorTickEnabled);
		SetActorEnableCollision(SpawnOptions.EnableCollision);
		//
		FinishSpawnFromPool(Reconstruct,SpawnTransform);
		if (!Spawned) {return;}
		//
		if (LifeSpanPool>0.0001f) {
			const FTimerDelegate Timer = FTimerDelegate::CreateUObject(this,&APooledPawn::ReturnToPool);
			GetWorld()->GetTimerManager().SetTimer(LifeSpanHandle,Timer,LifeSpanPool,false);
	} EVENT_OnPoolBeginPlay(); OnPoolBeginPlay.Broadcast();}
	//
	SetActorHiddenInGame(false);
	//
	for (const auto &C : Components) {
		const auto &P = Cast<UParticleSystemComponent>(C);
		if (P) {
			P->SetVisibility(true,true);
			P->Activate(true);
	P->ActivateSystem(true);}}
}

void APooledPawn::FinishSpawnFromPool(const bool Reconstruct, const FTransform &Transform) {
	FTransform FinalRootComponentTransform = (RootComponent ? RootComponent->GetComponentTransform() : Transform);
	//
	FinalRootComponentTransform.GetLocation().DiagnosticCheckNaN(TEXT("APooledPawn::FinishSpawning: FinalRootComponentTransform.GetLocation()"));
	FinalRootComponentTransform.GetRotation().DiagnosticCheckNaN(TEXT("APooledPawn::FinishSpawning: FinalRootComponentTransform.GetRotation()"));
	//
	if (GetWorld()) {
		FVector AdjustedLocation; FRotator AdjustedRotation;
		switch (SpawnCollisionHandlingMethod) {
			case ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn:
				AdjustedLocation = GetActorLocation();
				AdjustedRotation = GetActorRotation();
				if (GetWorld()->FindTeleportSpot(this,AdjustedLocation,AdjustedRotation)) {
					SetActorLocationAndRotation(AdjustedLocation,AdjustedRotation,false,nullptr,ETeleportType::TeleportPhysics);
			} break;
			//
			case ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding:	
				AdjustedLocation = GetActorLocation();
				AdjustedRotation = GetActorRotation();
				if (GetWorld()->FindTeleportSpot(this,AdjustedLocation,AdjustedRotation)) {
					SetActorLocationAndRotation(AdjustedLocation,AdjustedRotation,false,nullptr,ETeleportType::TeleportPhysics);
				} else {
					UE_LOG(LogTemp,Warning,TEXT("Spawn Actor from Pool: failed because of collision at the spawn location [%s] for [%s]"),*AdjustedLocation.ToString(),*GetClass()->GetName());
					Initialize();
				return;}
			break;
			//
			case ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding:
				if (GetWorld()->EncroachingBlockingGeometry(this,GetActorLocation(),GetActorRotation())) {
					UE_LOG(LogTemp,Warning,TEXT("Spawn Actor from Pool: failed because of collision at the spawn location [%s] for [%s]"),*GetActorLocation().ToString(),*GetClass()->GetName());
					Initialize();
				return;}
			break;
			//
			case ESpawnActorCollisionHandlingMethod::Undefined:
			case ESpawnActorCollisionHandlingMethod::AlwaysSpawn:
	default: break;}}
	//
	if (Reconstruct) {
		ResetPropertiesForConstruction();
		RerunConstructionScripts();
	Reset();}
}

void APooledPawn::ReturnToPool() {
	const auto &Settings = GetMutableDefault<UPoolSettings>();
	Spawned = false;
	//
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	//
	TInlineComponentArray<UActorComponent*> Components;
	GetComponents(Components);
	//
	for (const auto &C : Components) {
		const auto &P = Cast<UPrimitiveComponent>(C);
		if (P) {
			P->SetSimulatePhysics(false);
			P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			P->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			P->SetPhysicsLinearVelocity(FVector::ZeroVector);
			P->SetComponentTickEnabled(false);
			P->SetVisibility(false,true);
	} C->Deactivate();}
	//
	if (OwningPool && OwningPool->IsValidLowLevelFast()) {
		if (Settings->ReinitializeInstances) {
			GetWorld()->GetTimerManager().ClearTimer(LifeSpanHandle);
			OnPoolEndPlay.Broadcast();
			EVENT_OnPoolEndPlay();
		} OwningPool->ReturnActor(const_cast<APooledPawn*>(this));
	} else {Destroy();}
}

UPawnPool* APooledPawn::GetOwningPool() const {
	return OwningPool;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////