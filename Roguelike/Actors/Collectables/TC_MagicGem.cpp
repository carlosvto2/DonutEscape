// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_MagicGem.h"
#include "../../TC_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SlateWrapperTypes.h"

void ATC_MagicGem::BeginPlay()
{
	Super::BeginPlay();

	DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));
	
}

void ATC_MagicGem::PickUp()
{
	if (PickUpSound)
	{
		UGameplayStatics::PlaySound2D(this, PickUpSound, 1.0f, 1.0f);
	}

	if (!DonutGameMode)
		return;
	DonutGameMode->ToggleGemShop(ESlateVisibility::Visible);
}