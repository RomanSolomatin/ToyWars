// Fill out your copyright notice in the Description page of Project Settings.

#include "FlyShooter.h"
#include "WheelFront.h"


UWheelFront::UWheelFront(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ShapeRadius = 40.f;
	ShapeWidth = 10.0f;
	bAffectedByHandbrake = false;
	SteerAngle = 50.f;
}

