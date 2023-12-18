// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_AnimInstance.h"

UTC_AnimInstance::UTC_AnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GrabItem = false;
	ThrowItem = false;
}

void UTC_AnimInstance::GrabAnimation()
{
	GrabItem = true;
}

void UTC_AnimInstance::ThrowAnimation()
{
	ThrowItem = true;
}