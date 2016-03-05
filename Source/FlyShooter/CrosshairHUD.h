// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "CrosshairHUD.generated.h"

/**
 * 
 */
UCLASS()
class FLYSHOOTER_API ACrosshairHUD : public AHUD
{
	GENERATED_BODY()

public:
	ACrosshairHUD(const FObjectInitializer& ObjectInitializer);

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;
	
private:
	/** Crosshair asset pointer */
	UTexture2D* CrosshairTex;
	
};
