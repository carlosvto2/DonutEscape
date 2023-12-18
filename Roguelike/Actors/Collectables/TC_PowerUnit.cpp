// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_PowerUnit.h"


void ATC_PowerUnit::PickUp()
{
	if (PickUpSound)
	{
		UGameplayStatics::PlaySound2D(this, PickUpSound, 1.0f, 1.0f);
	}
	Destroy();
}