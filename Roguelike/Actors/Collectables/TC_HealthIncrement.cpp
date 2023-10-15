// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_HealthIncrement.h"
#include "Kismet/GameplayStatics.h"
#include "../../Characters/TC_DonutPlayer.h"

void ATC_HealthIncrement::BeginPlay()
{
	Super::BeginPlay();

	// Get a reference to the players pawn
	Donut = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void ATC_HealthIncrement::PickUp()
{
	if (PickUpSound)
	{
		UGameplayStatics::PlaySound2D(this, PickUpSound, 1.0f, 1.0f);
	}
	Donut->IncrementHealth(HealthIncrementation);
	Destroy();
}