// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_MultipleShoot.h"
#include "Kismet/GameplayStatics.h"
#include "../../Characters/TC_DonutPlayer.h"
#include "../../Environment/TC_Room.h"

void ATC_MultipleShoot::BeginPlay()
{
	Super::BeginPlay();

	// Get a reference to the players pawn
	Donut = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void ATC_MultipleShoot::PickUp()
{
	if (PickUpSound)
	{
		UGameplayStatics::PlaySound2D(this, PickUpSound, 1.0f, 1.0f);
	}
	Donut->MultipleShoot();
	if (GetOwner())
	{
		ATC_Room* OwnerRoom = Cast<ATC_Room>(GetOwner());
		OwnerRoom->NoBuffInRoom();
	}
	Destroy();
}