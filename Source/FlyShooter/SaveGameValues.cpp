// Fill out your copyright notice in the Description page of Project Settings.

#include "FlyShooter.h"
#include "SaveGameValues.h"

const int USaveGameValues::required_experience[] =
{
	0, 45, 95, 145, 210, 285, 380, 495, 610, 745, 99999999
};

USaveGameValues::USaveGameValues(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SaveSlotName = TEXT("TestSaveSlot");
	UserIndex = 0;
	PlayerGold = 0;
	PlayerCurrentXP = 0;
}

void USaveGameValues::AddXP(float xp)
{	
	PlayerCurrentXP += xp;
	PlayerAllXP += xp;
	UE_LOG(LogTemp, Warning, TEXT("Got %f xp"), xp);
	UE_LOG(LogTemp, Warning, TEXT("Gold: %f"), PlayerGold);
	UE_LOG(LogTemp, Warning, TEXT("CUrrent xp: %f"), PlayerCurrentXP);
	UE_LOG(LogTemp, Warning, TEXT("XP: %f, Level: %i"), PlayerCurrentXP, PlayerCurrentLevel);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("XP: %f"), PlayerCurrentXP));
	
	if (PlayerCurrentXP >= required_experience[PlayerCurrentLevel] && PlayerCurrentLevel < 10)
	{		
		PlayerCurrentXP -= required_experience[PlayerCurrentLevel];
		LevelUp();
	}

	CalcLevelProgress();
}

void USaveGameValues::CalcLevelProgress()
{
	PlayerCurrentProgressBarPercent =  PlayerCurrentXP / required_experience[PlayerCurrentLevel];
}

void USaveGameValues::LevelUp()
{
	PlayerCurrentLevel++;
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Level UP!!"));
}

void USaveGameValues::ReceiveGold(int gold)
{
	PlayerGold += gold;
	UE_LOG(LogTemp, Warning, TEXT("Gold: %i"), PlayerGold);
}

