// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_MagicBox.h"
#include "../../Characters/TC_DonutPlayer.h"
#include "../../ActorComponents/TC_HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../../TC_GameMode.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ATC_MagicBox::ATC_MagicBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ATC_MagicBox::BeginPlay()
{
	Super::BeginPlay();

	DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!DonutGameMode)
		return;
	
	// Get a reference to the players pawn
	Donut = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void ATC_MagicBox::PickUp()
{
	if (PickUpSound)
	{
		UGameplayStatics::PlaySound2D(this, PickUpSound, 1.0f, 1.0f);
	}
	UTC_HealthComponent* PlayerHealth = Donut->GetHealthComponent();
	
	// Player is full health
	if (PlayerHealth->GetHealth() == PlayerHealth->GetMaxHealth())
	{
		DonutGameMode->ShowHelpTextWidget("Player must not be full health to use the stone");
		return;
	}

	//Rotate character
	DonutGameMode->HandleMapRotation();

	// Set player full health and update widget
	PlayerHealth->SetHealth(PlayerHealth->GetMaxHealth());
}

