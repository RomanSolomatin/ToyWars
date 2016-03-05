// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "SaveGameValues.generated.h"

/**
 * 
 */
UCLASS()
class FLYSHOOTER_API USaveGameValues : public USaveGame
{
	GENERATED_BODY()
	
public:
	USaveGameValues(const FObjectInitializer& ObjectInitializer);

	static const int required_experience[];

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
		FString SaveSlotName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
		int32 UserIndex;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
		FString PlayerName;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
		int32 PlayerGold;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
		float PlayerCurrentXP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
		float PlayerAllXP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
		float PlayerCurrentProgressBarPercent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Basic)
		int32 PlayerCurrentLevel;


	/** Skill Properties for the Flying Pawn */
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = FlyingPawnSkill)	
		int32 FlyingPawn_CurrentAccelerationLevel;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = FlyingPawnSkill)
		int32 FlyingPawn_CurrentMaxSpeedLevel;

	/** Skill Properties for the Simple Car */
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = SimpleCarSkill)
		int32 SimpleCar_CurrentAccelerationLevel;

	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = SimpleCarSkill)
		int32 SimpleCar_CurrentMaxSpeedLevel;

	/** Skill properties for the mg projectile */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = MGProjectile)
		int32 MGProjectile_Damage;

	/** Skill properties for the rocket projectile */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = RocketProjectile)
		int32 RocketProjectile_Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = RocketProjectile)
		int32 RocketProjectile_MaxSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = RocketProjectile)
		bool RocketProjectile_Tracing;
public:
	//UFUNCTION()
	/** We call this whenever the player should be given experience */
	void AddXP(float xp);

	/** has the pawn obtained enough experience for a level? */
	void CalcLevelProgress();

	void LevelUp();

	void ReceiveGold(int gold);
};
