// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_Coin.h"
#include "../../TC_GameMode.h"
#include "Kismet/GameplayStatics.h"


void ATC_Coin::PickUp()
{
	if (PickUpSound)
	{
		UGameplayStatics::PlaySound2D(this, PickUpSound, 1.0f, 1.0f);
	}

	Destroy();
}