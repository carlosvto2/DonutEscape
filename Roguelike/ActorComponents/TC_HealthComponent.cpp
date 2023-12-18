// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_HealthComponent.h"
#include "../TC_GameMode.h"
#include "../Characters/TC_BaseCharacter.h"
#include "../Characters/TC_DonutPlayer.h"
#include "../Characters/TC_Boss.h"
#include "Kismet/GameplayStatics.h"
#include "../WidgetComponent/TC_WidgetComponent.h"

// Sets default values for this component's properties
UTC_HealthComponent::UTC_HealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UTC_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!DonutGameMode)
		return;


	// if not shield, full health
	if(!IsShield)
		Health = MaxHealth;
	
	// when there is any damage, call the function specified
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UTC_HealthComponent::DamageTaken);

	// Set health widget for Enemy
	if (HealthWidgetClass)
	{
		ATC_BaseCharacter* CharacterOwner = Cast<ATC_BaseCharacter>(GetOwner());
		EnemyHealthWidgetComponent = NewObject<UTC_WidgetComponent>(CharacterOwner, HealthWidgetClass);
		EnemyHealthWidgetComponent->InitializeEnemyHealthWidget(CharacterOwner, Health, MaxHealth);
	}
}


// Called every frame
void UTC_HealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTC_HealthComponent::SetHealth(float Value)
{
	Health = Value;

	if (Health > 100.f && Health > MaxHealth)
		Health = MaxHealth;

	if(!IsShield)
		DonutGameMode->UpdatePlayerHealthWidget();
	else
		DonutGameMode->UpdatePlayerShieldWidget();
}

void UTC_HealthComponent::SetMaxHealth(float Value)
{
	MaxHealth = Value;
}

void UTC_HealthComponent::SetComponentAsShield(bool Value)
{
	// set the component as shield, and health must be 0
	IsShield = Value;
	Health = 0.f;
}

// This function will be called every time the actor takes damage (TakeDamage())
void UTC_HealthComponent::DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser)
{
	// 
	// Dont continue when no damage, or the character is already dead
	if (Damage <= 0.f || CharacterDead || !DonutGameMode)
		return;

	// if the character has a shield
	ATC_BaseCharacter* Character = Cast<ATC_BaseCharacter>(GetOwner());
	UTC_HealthComponent* CharacterShield = Character->GetShieldIfExist();
	// if this component is not the character shield itself
	//if (CharacterShield && !IsShield)
	//{
	//	// substract to health the rest amount
	//	Damage -= CharacterShield->GetHealth();
	//}

	Health -= Damage;

	if (ATC_DonutPlayer* Player = Cast<ATC_DonutPlayer>(Character))
	{
		UE_LOG(LogTemp, Warning, TEXT("Damage %f"), Damage);
		// PLAYER
		DonutGameMode->UpdatePlayerHealthWidget();
		CreateDamageWidgetComponent(Damage * -1 , FLinearColor::Red);
	}
	else
	{
		// ENEMY
		CreateDamageWidgetComponent(Damage, FLinearColor::Yellow);
		if (EnemyHealthWidgetComponent)
		{
			// Update the enemy health
			EnemyHealthWidgetComponent->UpdateHealthWidget(Health, MaxHealth);
		}
	}

	/*if (ATC_Boss* Boss = Cast<ATC_Boss>(Character))
		DonutGameMode->UpdateBossHealthWidget(true, Boss);*/

	// if the health of the component is 0
	if (Health <= 0.0f)
	{
		// this component is not the shield
		if (!IsShield)
		{
			// call the actor died function in the gamemode
			DonutGameMode->ActorDied(GetOwner());
			CharacterDead = true;

			// Hide Health Bar Widget
			ToggleVisibilityHealthWidget(false);
		}
		else
		{
			// Update shield widget
			DonutGameMode->UpdatePlayerShieldWidget();
		}

		// if this is the character shield and is below 0, set it to 0
		Health = 0.f;
	}
}

/*
* It creates a widget component to show the damage the character is taking
*/
void UTC_HealthComponent::CreateDamageWidgetComponent(float Damage, FLinearColor DamageColor)
{
	if (DamageWidgetClass)
	{
		UTC_WidgetComponent* DamageWidgetComponent;
		ATC_BaseCharacter* CharacterOwner = Cast<ATC_BaseCharacter>(GetOwner());
		DamageWidgetComponent = NewObject<UTC_WidgetComponent>(CharacterOwner, DamageWidgetClass);
		DamageWidgetComponent->InitializeDamageWidget(CharacterOwner, Damage, DamageColor);
	}
}


void UTC_HealthComponent::ToggleVisibilityHealthWidget(bool Visible)
{
	if (!EnemyHealthWidgetComponent)
		return;
	EnemyHealthWidgetComponent->SetVisibility(Visible);
}