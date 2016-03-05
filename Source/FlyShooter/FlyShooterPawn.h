// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "GameFramework/Pawn.h"
#include "SaveGameValues.h"
#include "FlyShooterPawn.generated.h"

UCLASS(config=Game)
class AFlyShooterPawn : public APawn
{
	GENERATED_BODY()

	/** StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PlaneMesh;

	/** StaticMesh component that will be the visuals for our flying pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* PropellerMesh;

	/** Spring arm that will offset the camera */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArm;

	/** Camera component that will be our viewpoint */
	UPROPERTY(Category = Camera, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* Camera;

	/** Explosion particle at the collsion */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UParticleSystemComponent* SmokeParticleComp;

public:
	AFlyShooterPawn(const FObjectInitializer& ObjectInitializer);

	static const FName FireBinding;

	// Begin AActor overrides
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);
	// End AActor overrides

	/* Handler for the fire timer expiry */
	void ShotTimerExpired();

	/* How fast the weapon will fire */
	UPROPERTY(Category = Gameplay, EditAnywhere, BlueprintReadWrite)
		float FireRate;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class USoundBase* FireSound;

	/* Sets the acceleration do default value*/
	void SetAccelerationToDefault();



	/** engine sound */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundCue* EngineSound;

	/** audio component for engine sounds */
	UPROPERTY()
	UAudioComponent* EngineAC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerCondition)
	float health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = PlayerCondition)
	int32 quantityRockets;

	USaveGameValues* GameInstance;
protected:

	// Begin APawn overrides
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override; // Allows binding actions/axes to functions
	// End APawn overrides

	/** Bound to the thrust axis */
	void ThrustInput(float Val);
	
	/** Bound to the vertical axis */
	void MoveUpInput(float Val);

	/** Bound to the horizontal axis */
	void MoveRightInput(float Val);

	/** Fires a projectile. */
	void OnFire();

private:

	/** How quickly forward speed changes */
	UPROPERTY(Category=Plane, EditAnywhere)
	float Acceleration;

	/** How quickly pawn can steer */
	UPROPERTY(Category=Plane, EditAnywhere)
	float TurnSpeed;

	/** Max forward speed */
	UPROPERTY(Category = Pitch, EditAnywhere)
	float MaxSpeed;

	/** Min forward speed */
	UPROPERTY(Category=Yaw, EditAnywhere)
	float MinSpeed;

	/** Current forward speed */
	UPROPERTY(Category = Yaw, EditAnywhere)
	float CurrentForwardSpeed;

	/** Current yaw speed */
	float CurrentYawSpeed;

	/** Current pitch speed */
	float CurrentPitchSpeed;

	/** Current roll speed */
	float CurrentRollSpeed;

	/* Flag to control firing  */
	uint32 bCanFire : 1;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;

	/** Handle Acceleration Boni */
	FTimerHandle AccelerationBoniHandler;

	FVector2D rotate;

	int PropellerRotation;

	int currentWeapon;

	void SetWeaponMG();
	void SetWeaponRocket();

public:
	UFUNCTION(BlueprintCallable, Category = "Default")
		void SetMaxSpeed(float speed);

	UFUNCTION(BlueprintCallable, Category = "Default")
		float GetMaxSpeed();

	UFUNCTION(BlueprintCallable, Category = "Default")
		void SetAcceleration(float acc);

	UFUNCTION(BlueprintCallable, Category = "Default")
		float GetAcceleration();

	/** Returns PlaneMesh subobject **/
	FORCEINLINE class UStaticMeshComponent* GetPlaneMesh() const { return PlaneMesh; }
	/** Returns SpringArm subobject **/
	FORCEINLINE class USpringArmComponent* GetSpringArm() const { return SpringArm; }
	/** Returns Camera subobject **/
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }
	/** Returns EngineAC subobject **/
	FORCEINLINE UAudioComponent* GetEngineAC() const { return EngineAC; }
};
