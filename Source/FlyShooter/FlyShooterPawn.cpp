// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "FlyShooter.h"
#include "FlyShooterPawn.h"
#include "Projectile.h"
#include "RocketProjectile.h"
#include "TimerManager.h"

const FName AFlyShooterPawn::FireBinding("Fire");


AFlyShooterPawn::AFlyShooterPawn(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMeshAsset(TEXT("/Game/Flying/Meshes/Plane1/aircraft_final.aircraft_final"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> PropellerMeshAsset(TEXT("/Game/Flying/Meshes/Plane1/aircraft_final_propeller.aircraft_final_propeller"));

	// Create static mesh component
	PlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneMesh0"));
	PlaneMesh->SetStaticMesh(PlaneMeshAsset.Object);
	PlaneMesh->SetRelativeScale3D(FVector(0.5, 0.5, 0.5));
	
	PropellerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TowerRocketsMesh0"));
	PropellerMesh->SetStaticMesh(PropellerMeshAsset.Object);

	
	PropellerMesh->AttachTo(PlaneMesh, "Propeller");
	RootComponent = PlaneMesh;

	// Cache our sound effect
	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/Flying/Sounds/gunshot.gunshot"));
	FireSound = FireAudio.Object;
	

	// Create a spring arm component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm0"));
	SpringArm->AttachTo(RootComponent);
	SpringArm->TargetArmLength = 320.0f; // The camera follows at this distance behind the character	
	SpringArm->SocketOffset = FVector(0.f,0.f,60.f);
	SpringArm->bEnableCameraLag = false;
	SpringArm->CameraLagSpeed = 15.f;

	// Create camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera0"));
	Camera->AttachTo(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false; // Don't rotate camera with controller

	static ConstructorHelpers::FObjectFinder<UParticleSystem> SmokeParticle(TEXT("ParticleSystem'/Game/StarterContent/Particles/P_Smoke2.P_Smoke2'"));

	SmokeParticleComp = CreateDefaultSubobject<UParticleSystemComponent>("SmokeParticle0");

	if (SmokeParticle.Succeeded())
		SmokeParticleComp->Template = SmokeParticle.Object;

	SmokeParticleComp->bAutoActivate = false;
	SmokeParticleComp->SetHiddenInGame(false);
	
	SmokeParticleComp->AttachTo(PlaneMesh, "Smoke");

	// Set handling parameters
	Acceleration = 300.f; // 500
	TurnSpeed = 75.f;
	MaxSpeed = 1500.f; // 5000
	MinSpeed = 0.f; // 500
	CurrentForwardSpeed = 0.f; // 500
	
	EngineAC = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("EngineAudio"));

	FireRate = 0.15f;
	bCanFire = true;

	PropellerRotation = 0;

	health = 100.f;
	quantityRockets = 10;
	currentWeapon = 1;

	GameInstance = Cast<USaveGameValues>(UGameplayStatics::CreateSaveGameObject(USaveGameValues::StaticClass()));
	GameInstance = Cast<USaveGameValues>(UGameplayStatics::LoadGameFromSlot(GameInstance->SaveSlotName, GameInstance->UserIndex));
	/*UDataTable* table;
	static ConstructorHelpers::FObjectFinder<UDataTable> MaxSpeedTable(TEXT("DataTable'/Game/Data/MaxSpeedSkill.MaxSpeedSkill'"));


	if (MaxSpeedTable.Succeeded())
		table = MaxSpeedTable.Object;
	
	FString level = FString::FromInt(GameInstance->FlyingPawn_CurrentMaxSpeedLevel);
	table->FindRow<UDataTable>(FName(*level),FString(""));
	//MaxSpeed = ;*/
}

void AFlyShooterPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (EngineAC)
	{
		EngineAC->SetSound(EngineSound);
		EngineAC->Play();
	}
}

void AFlyShooterPawn::OnFire()
{
	if (bCanFire == true)
	{
		UE_LOG(LogTemp, Warning, TEXT("OnFire"));
		
		FVector NewLocation = PlaneMesh->GetSocketLocation("ShootRocketAt");
		FRotator NewRotation = GetActorRotation() + FRotator(0.f, 0.f, 0.f);

		UWorld* const World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = Instigator;


			if (currentWeapon == 2 && quantityRockets > 0)
			{
				FireRate = 0.15f;
				ARocketProjectile* const Projectile = World->SpawnActor<ARocketProjectile>(NewLocation, NewRotation, SpawnParams);

				if (Projectile)
				{
					Projectile->InitVelocity(GetActorForwardVector());
					quantityRockets--;
				}


			}
			else if (currentWeapon == 1)
			{
				FireRate = 0.05f;
				AProjectile* const Projectile = World->SpawnActor<AProjectile>(NewLocation, NewRotation, SpawnParams);

				if (Projectile)
				{
					Projectile->InitVelocity(GetActorForwardVector());
				}
			}
		}

		bCanFire = false;

		World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AFlyShooterPawn::ShotTimerExpired, FireRate);

		bCanFire = false;

	}
}

void AFlyShooterPawn::Tick(float DeltaSeconds)
{
	FVector GravityAcc = FVector(0.f, 0.f, -1.f) * 5.f;
	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f) + GravityAcc;

	// Move plan forwards (with sweep so we stop when we collide with things)
	AddActorLocalOffset(LocalMove, true);

	// Calculate change in rotation this frame
	FRotator DeltaRotation(0,0,0);
	DeltaRotation.Pitch = CurrentPitchSpeed * DeltaSeconds;
	DeltaRotation.Yaw = CurrentYawSpeed * DeltaSeconds;
	DeltaRotation.Roll = CurrentRollSpeed * DeltaSeconds;

	// Rotate plane
	AddActorLocalRotation(DeltaRotation, true);

	if(CurrentForwardSpeed > 500 && CurrentForwardSpeed < 1000)
		PropellerRotation += 5 + 0.66;

	if (CurrentForwardSpeed >= 1000 && CurrentForwardSpeed < 1500)
		PropellerRotation += 10 + 0.66;

	if (CurrentForwardSpeed >= 1500 && CurrentForwardSpeed < 2501)
		PropellerRotation += 15 + 0.66;

	PropellerMesh->SetRelativeRotation(FRotator(0.f, 0.f, PropellerRotation));

	if (PropellerRotation >= 360)
		PropellerRotation = 0;

	const float FireValue = GetInputAxisValue(FireBinding);

	if (FireValue == 1)
		OnFire();

	if (EngineAC){
		//UE_LOG(LogTemp, Warning, TEXT("VeloSize: %f"), CurrentForwardSpeed);
		EngineAC->SetPitchMultiplier(1 + CurrentForwardSpeed/MaxSpeed);
	}

	// Call any parent class Tick implementation
	Super::Tick(DeltaSeconds);
}

void AFlyShooterPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	
	//UE_LOG(LogTemp, Warning, TEXT("Collision with: %s"), *Other->GetName());

	if (Other->GetName().Contains("SpeedBoni"))
	{
		UE_LOG(LogTemp, Warning, TEXT("SpeedBoni started"));
		Other->Destroy();
		Acceleration = 5000;
		MaxSpeed = 2500;
		GetWorldTimerManager().SetTimer(AccelerationBoniHandler, this, &AFlyShooterPawn::SetAccelerationToDefault, 5, false);
	}
	else if (Other->GetName().Contains("RocketBoni"))
	{
		UE_LOG(LogTemp, Warning, TEXT("RocketBoni started"));
		Other->Destroy();
		quantityRockets += 5;
	}
	else if (Other->GetName().Contains("GoldBoni"))
	{
		UE_LOG(LogTemp, Warning, TEXT("GoldtBoni started"));
		Other->Destroy();
		
		GameInstance->ReceiveGold(50);
		UGameplayStatics::SaveGameToSlot(GameInstance, GameInstance->SaveSlotName, GameInstance->UserIndex);		
	}
	else
	{
		//SetActorLocation(GetActorLocation() +  GetActorForwardVector() * -200);

		/*HitNormal.Normalize();
		FVector NewForward = GetActorForwardVector() - 2 * HitNormal * (FVector::DotProduct(GetActorForwardVector(), HitNormal));
		FVector rAxis = FVector::CrossProduct(GetActorForwardVector(), NewForward);
		rAxis.Normalize();
		
		FVector fp = GetActorForwardVector();

		float angle = FMath::Acos(FVector::DotProduct(NewForward,fp));
		
		SetActorRotation(FRotator(FQuat(-rAxis, angle)));
		*/
		/* damp a velocity vector toward a surface
		
			Useful DampFactor is >=-1, where -1 will make Velocity parallel to the surface, 0 will do nothing, 
			and positive values will give a "pop up" effect by making the Velocity vector point further from the plane of the surface.
		*/
		//CurrentForwardSpeed += HitNormal * (Normal(Velocity) dot SurfaceNormal) * DampFactor;
	}
}

float AFlyShooterPawn::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("DamageCauser: %s, Amount: %f"), *DamageCauser->GetOwner()->GetName(), DamageAmount);

	health -= DamageAmount;
	
	if ( health < 50 )
		SmokeParticleComp->Activate(true);
	else
		SmokeParticleComp->Deactivate();


	return DamageAmount;
}

void AFlyShooterPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	// Bind our control axis' to callback functions
	InputComponent->BindAxis("Thrust", this, &AFlyShooterPawn::ThrustInput);
	InputComponent->BindAxis("MoveUp", this, &AFlyShooterPawn::MoveUpInput);
	InputComponent->BindAxis("MoveRight", this, &AFlyShooterPawn::MoveRightInput);
	InputComponent->BindAxis("Fire");
	InputComponent->BindAxis("LookUp", this, &AFlyShooterPawn::MoveUpInput);
	InputComponent->BindAxis("Turn", this, &AFlyShooterPawn::MoveRightInput);

	InputComponent->BindAction("WeaponMG", IE_Pressed, this, &AFlyShooterPawn::SetWeaponMG);
	InputComponent->BindAction("WeaponRocket", IE_Pressed, this, &AFlyShooterPawn::SetWeaponRocket);

	
}

void AFlyShooterPawn::ThrustInput(float Val)
{
	// Is there no input?
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);
	// If input is not held down, reduce speed
	float CurrentAcc = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);
	// Calculate new speed
	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcc);
	FString TheFloatStr = FString::SanitizeFloat(NewForwardSpeed);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, *TheFloatStr);
	// Clamp between MinSpeed and MaxSpeed
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, MinSpeed, MaxSpeed);
}

void AFlyShooterPawn::MoveUpInput(float Val)
{
	// Target pitch speed is based in input
	float TargetPitchSpeed = (Val * TurnSpeed * -1.f);

	// When steering, we decrease pitch slightly
	TargetPitchSpeed += (FMath::Abs(CurrentYawSpeed) * -0.2f);

	// Smoothly interpolate to target pitch speed
	CurrentPitchSpeed = FMath::FInterpTo(CurrentPitchSpeed, TargetPitchSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AFlyShooterPawn::MoveRightInput(float Val)
{
	// Target yaw speed is based on input
	float TargetYawSpeed = (Val * TurnSpeed);

	// Smoothly interpolate to target yaw speed
	CurrentYawSpeed = FMath::FInterpTo(CurrentYawSpeed, TargetYawSpeed, GetWorld()->GetDeltaSeconds(), 2.f);

	// Is there any left/right input?
	const bool bIsTurning = FMath::Abs(Val) > 0.2f;

	// If turning, yaw value is used to influence roll
	// If not turning, roll to reverse current roll value
	float TargetRollSpeed = bIsTurning ? (CurrentYawSpeed * 0.5f) : (GetActorRotation().Roll * -2.f);

	// Smoothly interpolate roll speed
	CurrentRollSpeed = FMath::FInterpTo(CurrentRollSpeed, TargetRollSpeed, GetWorld()->GetDeltaSeconds(), 2.f);
}

void AFlyShooterPawn::ShotTimerExpired()
{
	UE_LOG(LogTemp, Warning, TEXT("ShotTimerExpired"));

	bCanFire = true;
}

void AFlyShooterPawn::SetAccelerationToDefault()
{
	UE_LOG(LogTemp, Warning, TEXT("SpeedBoni ended"));

	Acceleration = 150;
	MaxSpeed = 1500;
}

void AFlyShooterPawn::SetWeaponMG()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Changed weapon to MG"));
	currentWeapon = 1;
}

void AFlyShooterPawn::SetWeaponRocket()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Changed weapon to Rockets"));
	currentWeapon = 2;
}

void AFlyShooterPawn::SetMaxSpeed(float speed)
{
	MaxSpeed = speed;
}

float AFlyShooterPawn::GetMaxSpeed()
{
	return MaxSpeed;
}

void AFlyShooterPawn::SetAcceleration(float acc)
{
	Acceleration = acc;
}

float AFlyShooterPawn::GetAcceleration()
{
	return Acceleration;
}