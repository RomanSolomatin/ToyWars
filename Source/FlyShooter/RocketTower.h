// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "RocketTower.generated.h"

UCLASS()
class FLYSHOOTER_API ARocketTower : public APawn
{
	GENERATED_BODY()

	/** StaticMesh component that will be the visuals for our tower pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* TowerMesh;

	/** StaticMesh component that will be the visuals for our tower pawn */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* TowerRockets;

	/** Sphere to check if Actor is in range */
	UPROPERTY(Category = Mesh, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* RangeSphere;

	/** Explosion particle at the collsion */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UParticleSystemComponent* SmokeParticleComp;
	
	/** Text component for the health status */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UTextRenderComponent* HealthStatus;

	/** called when something overlaps the sphere component */
	UFUNCTION()
		void OnBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
		void OnEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	// Sets default values for this pawn's properties
	ARocketTower(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// Called if got hitted
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser);

	// Fire function
	void ShootAtPlayer();

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class ARocketProjectile> RocketProjectile;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FireSound;

	/** The current health status */
	UPROPERTY(Category = Display, VisibleDefaultsOnly, BlueprintReadOnly)
		FText HealthStatusString;

	/** Handle fire of Rockets */
		FTimerHandle TimerHandle_ShotTimer;

private:	
	/** Max Health of Tower */
		float health;

	AActor* currentTarget;

};
