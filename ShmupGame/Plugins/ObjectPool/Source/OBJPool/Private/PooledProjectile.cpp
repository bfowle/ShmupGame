/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
///			Copyright 2018 (C) Bruno Xavier B. Leite
//////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PooledProjectile.h"
#include "OBJPoolShared.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UPooledProjectile::UPooledProjectile(const FObjectInitializer& OBJ) : Super(OBJ) {
	Movement = nullptr;
	//
	Direction = FVector::ForwardVector;
	InitialSpeed = 3000.0f;
	MaxSpeed = 3000.0f;
	//
	Friction = 0.2f;
	Bounciness = 0.6f;
	ForceSubStepping = false;
	MaxSimulationIterations = 8;
	ProjectileGravityScale = 1.f;
	UpdateOnlyIfRendered = false;
	MaxSimulationTimeStep = 0.05f;
	HomingAccelerationMagnitude = 0.f;
	InitialVelocityInLocalSpace = true;
	BounceAngleAffectsFriction = false;
	BounceVelocityStopSimulatingThreshold = 5.f;
}

void UPooledProjectile::PostLoad() {
	Super::PostLoad();
	//
	Owner = Cast<APooledActor>(GetOwner()); if (!Owner) {return;}
	//
	Owner->OnPoolBeginPlay.AddDynamic(this,&UPooledProjectile::Shoot);
	Owner->OnPoolEndPlay.AddDynamic(this,&UPooledProjectile::Break);
	//
	Owner->SetLifeSpan(0.f);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UPooledProjectile::Shoot_Implementation() {
	if (!Primitive) {
		Owner = Cast<APooledActor>(GetOwner());
		Primitive = Cast<UPrimitiveComponent>(Owner->GetRootComponent());
		//
		if (!Owner||!Owner->OwningPool||!Primitive) {
			UE_LOG(LogTemp,Error,TEXT("{Pool}:: %s"),TEXT("Pooled Projectile Components are meant to be used by ''Pooled Actor'' Class... but casting have failed or Owning Pool Component is invalid!"));
	return;}}
	//
	if (Direction==FVector::ZeroVector) {Direction = FVector::ForwardVector;}
	//
	if (Movement) {
		Movement->StopSimulating(FHitResult::FHitResult());
		Movement->DestroyComponent();
	Movement = nullptr;}
	//
	Movement = NewObject<UProjectileMovementComponent>(Owner);
	if (Movement) {
		Movement->BounceVelocityStopSimulatingThreshold = BounceVelocityStopSimulatingThreshold;
		Movement->bInitialVelocityInLocalSpace = InitialVelocityInLocalSpace;
		Movement->HomingAccelerationMagnitude = HomingAccelerationMagnitude;
		Movement->bBounceAngleAffectsFriction = BounceAngleAffectsFriction;
		Movement->bRotationFollowsVelocity = RotationFollowsVelocity;
		Movement->MaxSimulationIterations = MaxSimulationIterations;
		Movement->ProjectileGravityScale = ProjectileGravityScale;
		Movement->HomingTargetComponent = HomingTargetComponent;
		Movement->MaxSimulationTimeStep = MaxSimulationTimeStep;
		Movement->bUpdateOnlyIfRendered = UpdateOnlyIfRendered;
		Movement->bIsHomingProjectile = IsHomingProjectile;
		Movement->bForceSubStepping = ForceSubStepping;
		Movement->bShouldBounce = ShouldBounce;
		Movement->InitialSpeed = InitialSpeed;
		Movement->Bounciness = Bounciness;
		Movement->Velocity = Direction;
		Movement->Friction = Friction;
		Movement->MaxSpeed = MaxSpeed;
		//
		if (HomingTarget && HomingTarget->GetRootComponent()) {
			Movement->HomingTargetComponent = HomingTarget->GetRootComponent();
		} Movement->RegisterComponent();
		//
		Movement->OnProjectileStop.AddDynamic(this,&UPooledProjectile::ProjectileStop);
		Movement->OnProjectileBounce.AddDynamic(this,&UPooledProjectile::ProjectileBounce);
	}
}

void UPooledProjectile::Break_Implementation() {
	if ((!Owner||!Primitive||!Movement)||Owner->IsPendingKill()||!Owner->IsValidLowLevelFast()) {return;}
	//
	Primitive->SetPhysicsLinearVelocity(FVector::ZeroVector);
	//
	if (Movement) {
		Movement->OnProjectileStop.Clear();
		Movement->OnProjectileBounce.Clear();
		Movement->StopSimulating(FHitResult::FHitResult());
		Movement->DestroyComponent();
	}
}

void UPooledProjectile::ProjectileBounce(const FHitResult &HitResult, const FVector &Velocity) {
	OnProjectileBounce.Broadcast(HitResult,Velocity);
}

void UPooledProjectile::ProjectileStop(const FHitResult &HitResult) {
	OnProjectileStop.Broadcast(HitResult);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

UProjectileMovementComponent* UPooledProjectile::GetMovementComponent() const {
	return Movement;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////