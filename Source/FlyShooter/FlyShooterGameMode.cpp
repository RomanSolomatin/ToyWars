// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "FlyShooter.h"
#include "FlyShooterGameMode.h"
#include "FlyShooterPawn.h"
#include "SimpleCar.h"
#include "CrosshairHUD.h"

AFlyShooterGameMode::AFlyShooterGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// set default pawn class to our flying pawn
	//DefaultPawnClass = AFlyShooterPawn::StaticClass();
	DefaultPawnClass = ASimpleCar::StaticClass();
	HUDClass = ACrosshairHUD::StaticClass();
}
