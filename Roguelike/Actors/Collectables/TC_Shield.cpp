// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_Shield.h"
#include "Kismet/GameplayStatics.h"
#include "../../Characters/TC_DonutPlayer.h"
#include "../../ActorComponents/TC_HealthComponent.h"

void ATC_Shield::BeginPlay()
{
	Super::BeginPlay();

	// Get a reference to the players pawn
	Donut = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void ATC_Shield::PickUp()
{
	if (PickUpSound)
	{
		UGameplayStatics::PlaySound2D(this, PickUpSound, 1.0f, 1.0f);
	}
	// if the shield is already full, cannot take more
	if (Donut->GetShieldComponent()->GetHealth() == 100.f)
		return;

	Donut->IncrementShield(ShieldIncrementation);
	Destroy();
}