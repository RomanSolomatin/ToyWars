// Fill out your copyright notice in the Description page of Project Settings.

#include "FlyShooter.h"
#include "FlyShooterPawn.h"
#include "RocketTower.h"
#include "RocketProjectile.h"
#include "Projectile.h"
#include "TimerManager.h"

// Sets default values
ARocketTower::ARocketTower(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> RocketTowerMeshAsset(TEXT("/Game/Flying/Meshes/Tower/RocketTower-2_Scheibe.RocketTower-2_Scheibe"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> RocketTowerRocketsMeshAsset(TEXT("/Game/Flying/Meshes/Tower/RocketTower-2_Kegel_2.RocketTower-2_Kegel_2"));

	// Create static mesh component
	TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh0"));
	TowerMesh->SetStaticMesh(RocketTowerMeshAsset.Object); 
	TowerMesh->SetRelativeScale3D(FVector(15.f, 15.f, 15.f));
	TowerRockets = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerRocketsMesh0"));
	TowerRockets->SetStaticMesh(RocketTowerRocketsMeshAsset.Object);


	TowerRockets->AttachTo(TowerMesh, "RocketHolder");



	RangeSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("RangeSphere"));
	RangeSphere->InitSphereRadius(3000.0f);
	RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &ARocketTower::OnBeginOverlap);
	RangeSphere->OnComponentEndOverlap.AddDynamic(this, &ARocketTower::OnEndOverlap);

	RootComponent = RangeSphere;
	TowerMesh->AttachTo(RootComponent);
	
	RangeSphere->BodyInstance.SetCollisionProfileName("Custom");
	RangeSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	

	health = 100;
	currentTarget = nullptr;

	// Create text render component for health status
	HealthStatus = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarSpeed"));
	HealthStatus->AttachTo(TowerRockets);
	HealthStatus->SetRelativeLocation(FVector(15.f, 0.f, 10.f));
	HealthStatus->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));
	HealthStatus->SetText(FText::AsNumber(health));
}

// Called when the game starts or when spawned
void ARocketTower::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARocketTower::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (currentTarget != nullptr)
	{
		FVector a = this->GetActorLocation();
		FVector b = currentTarget->GetActorLocation();
		FVector v = b - a;
		v.Normalize();
		TowerRockets->SetRelativeRotation(v.Rotation());
		//HealthStatus->SetRelativeRotation(v.Rotation());
	}
}

// Called to bind functionality to input
void ARocketTower::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

void ARocketTower::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Hitted from %s"), *Other->GetName());
}

float ARocketTower::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("DamageCauser: %s, Amount: %f"), *DamageCauser->GetOwner()->GetName(), DamageAmount);

	health -= DamageAmount;
	HealthStatus->SetText(FText::AsNumber(health));

	UE_LOG(LogTemp, Warning, TEXT("Left health: %f"), health);


	if (health <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tower is dead"));		

		if (DamageCauser->GetOwner()->GetName().Contains("FlyShooterPawn"))
		{
			USaveGameValues* GameInstance;
			GameInstance = Cast<USaveGameValues>(UGameplayStatics::CreateSaveGameObject(USaveGameValues::StaticClass()));
			GameInstance = Cast<USaveGameValues>(UGameplayStatics::LoadGameFromSlot(GameInstance->SaveSlotName, GameInstance->UserIndex));
			GameInstance->AddXP(25.f);
			UGameplayStatics::SaveGameToSlot(GameInstance, GameInstance->SaveSlotName, GameInstance->UserIndex);

			/*AFlyShooterPawn* pawn = Cast<AFlyShooterPawn>(DamageCauser->GetOwner());

			if (pawn != nullptr)
			{
				pawn->GameInstance->AddXP(25.f);
				UGameplayStatics::SaveGameToSlot(pawn->GameInstance, pawn->GameInstance->SaveSlotName, pawn->GameInstance->UserIndex);

			}*/
		}

		Destroy();
	}

	return DamageAmount;

}

void ARocketTower::OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Actor is in Range Name: %s"), *OtherActor->GetName());
	if (OtherActor->Tags.Contains("Player"))
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			UE_LOG(LogTemp, Warning, TEXT("New Target: %s"), *OtherActor->GetName());
			currentTarget = OtherActor;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimer, this, &ARocketTower::ShootAtPlayer, 2.0F, true);
		}
	}
}

void ARocketTower::OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Actor is out of range Name: %s"), *OtherActor->GetName());
	if (OtherActor->Tags.Contains("Player"))
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			UE_LOG(LogTemp, Warning, TEXT("Delete Target: %s"), *OtherActor->GetName());
			World->GetTimerManager().ClearTimer(TimerHandle_ShotTimer);
			currentTarget = nullptr;
		}
	}
}

void ARocketTower::ShootAtPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("OnFire"));
	
	FVector NewLocation = TowerRockets->GetSocketLocation("RocketHolder") + (FVector(0.f, 0.f, 350.f));
	FRotator NewRotation = GetActorRotation();
	//currentTarget->GetInstigatorController()->GetHorizontalDistanceTo
	//SetActorRotation(v.Rotation());
	//UE_LOG(LogTemp, Warning, TEXT("NewLocation %s"), *NewLocation.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("NewRotation %s"), *NewRotation.ToString());

	UWorld* const World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		// spawn the projectile at the muzzle
		//AProjectile* const Projectile = World->SpawnActor<AProjectile>(MuzzleLocation, MuzzleRotation, SpawnParams);
		ARocketProjectile* const Projectile = World->SpawnActor<ARocketProjectile>(NewLocation, NewRotation, SpawnParams);

		if (Projectile)
		{
			Projectile->GetProjectileMovement()->HomingTargetComponent = currentTarget->GetRootComponent();
			
			FVector a = Projectile->GetActorLocation();
			FVector b = currentTarget->GetActorLocation();
			FVector v = b - a;
			v.Normalize();
			Projectile->GetProjectileMovement()->HomingAccelerationMagnitude = 200.f;
			Projectile->InitVelocity(v);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}