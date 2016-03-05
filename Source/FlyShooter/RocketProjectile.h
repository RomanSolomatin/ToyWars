// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "RocketProjectile.generated.h"

#define COLLISION_PROJECTILE    ECC_GameTraceChannel1

class UProjectileMovementComponent;
class UStaticMeshComponent;

UCLASS(config = Game)
class FLYSHOOTER_API ARocketProjectile : public AActor
{

	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Projectile, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* ProjectileMesh;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		UProjectileMovementComponent* ProjectileMovement;

	/** collisions */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		USphereComponent* CollisionComp;

	/** Explosion particle at the collsion */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
		UParticleSystemComponent* ExplosionComp;
	
public:

	ARocketProjectile(const FObjectInitializer& ObjectInitializer);

	/** Sound to play when missile is starting */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* FlySound;

	/** Sound to play when missile explodes on collision */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class USoundBase* Explosion;

	/** Sound to play when missile explodes on collision */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAudioComponent* AudioPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		class UAudioComponent* AudioPlayer1;

	/** Function to handle the projectile hitting something */
	UFUNCTION()
	void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** inits velocity of the projectile in the shoot direction */
	void InitVelocity(const FVector& ShootDirection);

	/** Returns ProjectileMesh subobject **/
	FORCEINLINE UStaticMeshComponent* GetProjectileMesh() const { return ProjectileMesh; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
	/** Returns ParticleComp subobject **/
	FORCEINLINE UParticleSystemComponent* GetExplosionComp() const { return ExplosionComp; }
};

