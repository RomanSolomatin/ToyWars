// Fill out your copyright notice in the Description page of Project Settings.

#include "FlyShooter.h"
#include "MGTower.h"
#include "FlyShooterPawn.h"
#include "Projectile.h"
#include "TimerManager.h"

AMGTower::AMGTower(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MGTowerMeshAsset(TEXT("/Game/Flying/Meshes/Tower/MG/MGTower_Tower.MGTower_Tower"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MGTowerRocketsMeshAsset(TEXT("/Game/Flying/Meshes/Tower/MG/MGTower_Cannons.MGTower_Cannons"));

	// Create static mesh component
	TowerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerMesh0"));
	TowerMesh->SetStaticMesh(MGTowerMeshAsset.Object);
	TowerMesh->SetRelativeScale3D(FVector(15.f, 15.f, 15.f));
	TowerCannons = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerCannonsMesh0"));
	TowerCannons->SetStaticMesh(MGTowerRocketsMeshAsset.Object);


	TowerCannons->AttachTo(TowerMesh, "Cannons");



	RangeSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("RangeSphere"));
	RangeSphere->InitSphereRadius(3000.0f);
	RangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AMGTower::OnBeginOverlap);
	RangeSphere->OnComponentEndOverlap.AddDynamic(this, &AMGTower::OnEndOverlap);

	RootComponent = RangeSphere;
	TowerMesh->AttachTo(RootComponent);

	RangeSphere->BodyInstance.SetCollisionProfileName("Custom");
	RangeSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);


	health = 100;
	currentTarget = nullptr;

	// Create text render component for health status
	HealthStatus = CreateDefaultSubobject<UTextRenderComponent>(TEXT("HealthStatus"));
	HealthStatus->AttachTo(TowerCannons);
	HealthStatus->SetRelativeLocation(FVector(15.f, 0.f, 10.f));
	HealthStatus->SetRelativeScale3D(FVector(1.0f, 0.4f, 0.4f));
	HealthStatus->SetText(FText::AsNumber(health));

	CanShoot = false;
}

// Called when the game starts or when spawned
void AMGTower::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AMGTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (currentTarget != nullptr)
	{
		FVector a = TowerCannons->GetSocketLocation("Cannons");
		FVector b = currentTarget->GetActorLocation();
		FVector v = b - a;
		//v.Normalize();
		bool GoodAngle = (v.Rotation().Pitch > -10) ? true : false;

		if (GoodAngle)
		{
			CanShoot = true;
			TowerCannons->SetRelativeRotation(FRotator(v.Rotation().Pitch, v.Rotation().Yaw, 0.f));
		}
		else{ CanShoot = false; }
		
		//HealthStatus->SetRelativeRotation(v.Rotation());
		
		// Pitch yaw roll
	}
}

// Called to bind functionality to input
void AMGTower::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);
}

void AMGTower::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Hitted from %s"), *Other->GetName());
}

float AMGTower::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
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
			GameInstance->AddXP(15.f);
			UGameplayStatics::SaveGameToSlot(GameInstance, GameInstance->SaveSlotName, GameInstance->UserIndex);
		}

		Destroy();
	}

	return DamageAmount;
}

void AMGTower::OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Actor is in Range Name: %s"), *OtherActor->GetName());
	if (OtherActor->Tags.Contains("Player"))
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			UE_LOG(LogTemp, Warning, TEXT("New Target: %s"), *OtherActor->GetName());
			currentTarget = OtherActor;
			World->GetTimerManager().SetTimer(TimerHandle_ShotTimer, this, &AMGTower::ShootAtPlayer, 0.05F, true);
		}
	}
}

void AMGTower::OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void AMGTower::ShootAtPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("OnFire"));

	FVector NewLocation = TowerCannons->GetSocketLocation("Cannons") + (FVector(0.f, 0.f, 400.f));
	FRotator NewRotation = GetActorRotation();
	//currentTarget->GetInstigatorController()->GetHorizontalDistanceTo
	//SetActorRotation(v.Rotation());
	//UE_LOG(LogTemp, Warning, TEXT("NewLocation %s"), *NewLocation.ToString());
	//UE_LOG(LogTemp, Warning, TEXT("NewRotation %s"), *NewRotation.ToString());

	if (CanShoot)
	{
		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;
			// spawn the projectile at the muzzle
			//AProjectile* const Projectile = World->SpawnActor<AProjectile>(MuzzleLocation, MuzzleRotation, SpawnParams);
			AProjectile* const Projectile = World->SpawnActor<AProjectile>(NewLocation, NewRotation, SpawnParams);

			if (Projectile)
			{
				Projectile->GetProjectileMovement()->HomingTargetComponent = currentTarget->GetRootComponent();
				FVector a = Projectile->GetActorLocation();
				FVector b = currentTarget->GetActorLocation();
				FVector v = b - a;
				v.Normalize();
				Projectile->InitVelocity(v);
			}
		}
	}
	// try and play the sound if specified
	/*if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}*/
}