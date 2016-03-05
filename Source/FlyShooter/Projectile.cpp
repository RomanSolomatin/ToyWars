// Fill out your copyright notice in the Description page of Project Settings.

#include "FlyShooter.h"
#include "Projectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AProjectile::AProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	RootComponent = CollisionComp;

	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/Flying/Meshes/Projectiles/Projectile.Projectile"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->AttachTo(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AProjectile::OnHit);		// set up a notification for when this component hits something
	ProjectileMesh->AttachParent = RootComponent;


	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 30000.f;
	ProjectileMovement->MaxSpeed = 30000.f; // 30000
	ProjectileMovement->bRotationFollowsVelocity = false;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	ParticleComp = ObjectInitializer.CreateDefaultSubobject<UParticleSystemComponent>(this, TEXT("ParticleComp"));
	ParticleComp->bAutoActivate = true;
	ParticleComp->bAutoDestroy = true;
	ParticleComp->AttachParent = RootComponent;
	
	AudioPlayerShot = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioPlayer"));
	static ConstructorHelpers::FObjectFinder<USoundBase> ShotSound(TEXT("/Game/Flying/Sounds/MachineGunShot.MachineGunShot"));
	Shot = ShotSound.Object;

	AudioPlayerShot->bStopWhenOwnerDestroyed = false;
	AudioPlayerShot->SetSound(Shot);

	AudioPlayerShot->Play();
	
	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void AProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Hit!!"));
	UE_LOG(LogTemp, Warning, TEXT("%s hat %s getroffen"),*this->GetName(), *OtherActor->GetName());
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 1000.0f, GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("Es wurde ein Objekt getroffen: %s"), *OtherComp->GetDetailedInfo());

		OtherActor->Destroy();
	}

	
	FPointDamageEvent DamageEvent;
	DamageEvent.HitInfo = Hit;
	DamageEvent.DamageTypeClass = UDamageType::StaticClass();


	if (Hit.Actor != NULL)
		Hit.Actor->TakeDamage(0.5, DamageEvent, Instigator->GetController(), this);

	Destroy();
}

void AProjectile::InitVelocity(const FVector& ShootDirection)
{
	if (ProjectileMovement)
	{
		// set the projectile's velocity to the desired direction
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
	}
}

