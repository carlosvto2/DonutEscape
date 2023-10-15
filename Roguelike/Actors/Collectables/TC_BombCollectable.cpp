// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_BombCollectable.h"
#include "Kismet/GameplayStatics.h"
#include "../../Characters/TC_DonutPlayer.h"

void ATC_BombCollectable::BeginPlay()
{
	Super::BeginPlay();
	SetActorRelativeRotation(FRotator(0.f, 0.f, -30.f));

	// Get a reference to the players pawn
	Donut = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void ATC_BombCollectable::PickUp()
{
	if (PickUpSound)
	{
		UGameplayStatics::PlaySound2D(this, PickUpSound, 1.0f, 1.0f);
	}
	Donut->IncrementBombs(NumberOfBombsIncrement);
	Destroy();
}