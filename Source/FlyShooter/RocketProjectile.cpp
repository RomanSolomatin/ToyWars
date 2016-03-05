// Fill out your copyright notice in the Description page of Project Settings.

#include "FlyShooter.h"
#include "RocketProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ARocketProjectile::ARocketProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	CollisionComp = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	RootComponent = CollisionComp;

	// Static reference to the mesh to use for the projectile
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/Flying/Meshes/Projectiles/Rocket/Rocket.Rocket"));

	// Create mesh component for the projectile sphere
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh0"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	ProjectileMesh->AttachTo(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	//ProjectileMesh->SetCollisionResponseToAllChannels(ECR_Block);
	ProjectileMesh->OnComponentHit.AddDynamic(this, &ARocketProjectile::OnHit);		// set up a notification for when this component hits something
	ProjectileMesh->AttachParent = RootComponent;


	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement0"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 4000.f;
	ProjectileMovement->MaxSpeed = 6000.f; // 30000
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f; // No gravity

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ExplosionParticle(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Explosion.P_Explosion'"));

	ExplosionComp = CreateDefaultSubobject<UParticleSystemComponent>("ExplosionParticle0");

	if (ExplosionParticle.Succeeded())
		ExplosionComp->Template = ExplosionParticle.Object;

	ExplosionComp->bAutoActivate = false;
	ExplosionComp->SetHiddenInGame(false);
	ExplosionComp->AttachTo(ProjectileMesh, "Explosion");

	AudioPlayer = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioPlayer"));
	AudioPlayer1 = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioPlayer1"));
	static ConstructorHelpers::FObjectFinder<USoundBase> FlyingSound(TEXT("/Game/Flying/Sounds/missile_flying.missile_flying"));
	FlySound = FlyingSound.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> ExplosionSound(TEXT("/Game/Flying/Sounds/Explosion_2.Explosion_2"));
	Explosion = ExplosionSound.Object;

	
	
	InitialLifeSpan = 0.0f;
	AudioPlayer->bAutoActivate = false;
	AudioPlayer1->bAutoActivate = false;
	AudioPlayer1->bStopWhenOwnerDestroyed = false;
	AudioPlayer->bStopWhenOwnerDestroyed = false;
	AudioPlayer->SetSound(FlySound);
	AudioPlayer1->SetSound(Explosion);

	AudioPlayer->Play();
	
}

void ARocketProjectile::OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	FString ActorName = this->GetOwner()->GetName();
	
	UE_LOG(LogTemp, Warning, TEXT("%s hat %s getroffen kam von: %s"), *this->GetName(), *OtherActor->GetName(), *ActorName);
	
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != NULL) && (OtherActor != this) && (OtherComp != NULL) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 1000.0f, GetActorLocation());
		UE_LOG(LogTemp, Warning, TEXT("Es wurde ein Objekt getroffen: %s"), *OtherComp->GetDetailedInfo());

		//OtherActor->Destroy();
	}

	

	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionComp->Template, Hit.Location, FRotator(0.f,0.f,0.f));

	// Explosion Sound
	UE_LOG(LogTemp, Warning, TEXT("ExplosionSound"));
	AudioPlayer1->Play();

	FPointDamageEvent DamageEvent;
	DamageEvent.HitInfo = Hit;
	DamageEvent.DamageTypeClass = UDamageType::StaticClass();


	if (Hit.Actor != NULL)
		Hit.Actor->TakeDamage(25.f, DamageEvent, Instigator->GetController(), this);

	Destroy();
}

void ARocketProjectile::InitVelocity(const FVector& ShootDirection)
{
	
	// set the projectile's velocity to the desired direction
	if (ProjectileMovement)	
		ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
}

