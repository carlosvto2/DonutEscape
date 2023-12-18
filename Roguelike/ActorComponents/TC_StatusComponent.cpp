// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_StatusComponent.h"
#include "../Effects/TC_SideEffect.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/TC_DonutPlayer.h"
#include "../Characters/TC_BaseCharacter.h"
#include "../TC_GameMode.h"
#include "../Actors/TC_Projectile.h"

// Sets default values for this component's properties
UTC_StatusComponent::UTC_StatusComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTC_StatusComponent::BeginPlay()
{
	Super::BeginPlay();

	DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));

	if (!DonutGameMode)
		return;


	PowerUnits = 0;

	CharacterOwner = Cast<ATC_BaseCharacter>(GetOwner());
	InitializePlayerSkinsPowerUnits();
}

void UTC_StatusComponent::InitializePlayerSkinsPowerUnits()
{
	//Initialize Skin Power Units
	DonutGameMode->UpdateSkinsPowerUnits(FireSkinPowerUnits, MaxFireSkinPowerUnits, WaterSkinPowerUnits,
		MaxWaterSkinPowerUnits, PlantSkinPowerUnits, MaxPlantSkinPowerUnits);

	UpdatePowerUnitsInPossession(0);
}

void UTC_StatusComponent::UpdatePowerUnitsInPossession(int Units)
{
	PowerUnits += Units;
	DonutGameMode->UpdatePowerUnitsValue(PowerUnits);
}


/*
* Function to increment the skin power units, pay the price and update the widget
* @param ItemBought: item bought by the player from the shop
* @param PowerUnitsToPay: Number of power units to be paid for the item
* @param MoneyToPay: Amount of money to be paid for the item
*/
void UTC_StatusComponent::UpdateItemsPlayer(FText ItemBought, int PowerUnitsToPay, int MoneyToPay)
{
	// Increment the skin power unit depending on the item bought
	if (ItemBought.ToString() == FText().FromString("Power Unit Of Fire").ToString())
	{
		FireSkinPowerUnits++;
	}
	else if (ItemBought.ToString() == FText().FromString("Power Unit Of Water").ToString())
	{
		WaterSkinPowerUnits++;
	}
	else if (ItemBought.ToString() == FText().FromString("Power Unit Of Plant").ToString())
	{
		PlantSkinPowerUnits++;
	}

	// Update the skin power units of the player
	DonutGameMode->UpdateSkinsPowerUnits(FireSkinPowerUnits, MaxFireSkinPowerUnits, WaterSkinPowerUnits,
		MaxWaterSkinPowerUnits, PlantSkinPowerUnits, MaxPlantSkinPowerUnits);
	// Update the Power units in possession
	UpdatePowerUnitsInPossession(-PowerUnitsToPay);
}


// Called every frame
void UTC_StatusComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


void UTC_StatusComponent::CheckIfSideEffectExists(TSubclassOf<ATC_SideEffect> SideEffectToApply, ATC_SideEffect*& SideEffect)
{
	if (CharacterEffects.Num() == 0)
		return;

	for (ATC_SideEffect* Effect : CharacterEffects)
	{
		if (Effect->IsA(SideEffectToApply))
		{
			SideEffect = Effect;
		}
	}
}

/*
* Add Effect to the status of the player
*/
void UTC_StatusComponent::AddSideEffect(FUProjectileInformation ProjectileRecovered)
{
	if (ProjectileRecovered.GetSideEffectAfterRecover().GetDefaultObject()->GetEffectType() 
		== EEffectType::Stack)
	{
		// if the type of the effect is stacks
		AddSideEffectStacks(ProjectileRecovered);
	}
}

/*
* Add Effect with stack increment
* @param ProjectileRecovered: information of the projectile recovered
*/
void UTC_StatusComponent::AddSideEffectStacks(FUProjectileInformation ProjectileRecovered)
{
	if (!ProjectileRecovered.GetSideEffectAfterRecover())
		return;

	ATC_SideEffect* SideEffect = nullptr;
	CheckIfSideEffectExists(ProjectileRecovered.GetSideEffectAfterRecover(), SideEffect);

	// if no plant effect is already applied to the character
	if (!SideEffect)
	{
		// Create side effect
		SideEffect = GetWorld()->SpawnActor<ATC_SideEffect>(ProjectileRecovered.GetSideEffectAfterRecover());
		// Set the character as the owner
		SideEffect->SetOwner(GetOwner());
		// Save in the effect the projectile used
		SideEffect->SetProjectileAssigned(ProjectileRecovered);
		// Add side effect to array of character effects
		CharacterEffects.Add(SideEffect);
	}
	else
	{
		SideEffect->IncrementStacks();
	}

	// Get the number of stacks and percentage damage of the effect
	int SideEffectStacks = SideEffect->GetStacks();
	float SideEffectDamagePercentage = SideEffect->GetDamagePercentage();

	// Add Side Effect to HUD
	if (DonutGameMode)
	{
		if (!SideEffect)
			return;
		DonutGameMode->AddSideEffectWidget(SideEffect->GetEffectTexture(), SideEffectStacks);
	}

	ATC_DonutPlayer* DonutPlayer = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (DonutPlayer)
	{
		// update the projectile Stacks
		DonutPlayer->UpdateProjectileStacks(ProjectileRecovered, SideEffectStacks,
			SideEffectDamagePercentage);
	}
	
}

void UTC_StatusComponent::RemoveSideEffect(ATC_SideEffect* SideEffectToRemove)
{
	// Remove the side effect from the array
	CharacterEffects.Remove(SideEffectToRemove);

	if (DonutGameMode)
	{
		if (!SideEffectToRemove)
			return;
		DonutGameMode->RemoveSideEffectWidget(SideEffectToRemove->GetEffectTexture());
	}

	// If the effect is type stack, update the projectile assigned
	if (SideEffectToRemove->GetEffectType() == EEffectType::Stack)
	{
		// Remove extra buff from Projectile
		ATC_DonutPlayer* DonutPlayer = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
		if (DonutPlayer)
		{
			DonutPlayer->UpdateProjectileStacks(SideEffectToRemove->GetProjectileAssignedInfo(),
				SideEffectToRemove->GetStacks(), SideEffectToRemove->GetDamagePercentage());
		}
	}
	
}



/*
* IF CHARACTER IS PLAYER - Check the type of projectile and skin and apply side effects to PLAYER
*/
float UTC_StatusComponent::CalculateAmmountOfDamageToPlayer(ATC_Projectile* Projectile)
{
	float ProjectileDamage = Projectile->GetDamage();

	float TotalDamage = 1;

	if (Projectile->GetProjectileType() == EProjectile::Fire)
	{
		switch (CharacterOwner->GetCharacterSkin())
		{
			float FireDamageToFireSkin;
			float FireDamageToWaterSkin;
			float FireDamageToPlantSkin;

		case ESkin::Fire:
			FireDamageToFireSkin = ProjectileDamage - (ProjectileDamage * DamageReductionWithSameSkin *
				GetFireSkinPowerUnits());
			TotalDamage = FireDamageToFireSkin;
			break;
		case ESkin::Water:
			FireDamageToWaterSkin = ProjectileDamage - (ProjectileDamage * DamageReductionWithSuferiorSkin *
				GetWaterSkinPowerUnits());
			TotalDamage = FireDamageToWaterSkin;
			break;
		case ESkin::Plant:
			// The more powerful is the plant skin, the more damage, the fire projectile does
			FireDamageToPlantSkin = ProjectileDamage + (ProjectileDamage * DamageIncrementWithInferiorSkin *
				GetPlantSkinPowerUnits());
			TotalDamage = FireDamageToPlantSkin;
			// CHECK
			//MakeDamageThroughTime();
			break;
		}
	}
	else if (Projectile->GetProjectileType() == EProjectile::Water)
	{
		switch (CharacterOwner->GetCharacterSkin())
		{
			float WaterDamageToFireSkin;
			float WaterDamageToWaterSkin;
			float WaterDamageToPlantSkin;

		case ESkin::Fire:
			WaterDamageToFireSkin = ProjectileDamage + (ProjectileDamage * DamageIncrementWithInferiorSkin *
				GetFireSkinPowerUnits());
			TotalDamage = WaterDamageToFireSkin;
			// reduce movement speed
			break;
		case ESkin::Water:
			WaterDamageToWaterSkin = ProjectileDamage - (ProjectileDamage * DamageReductionWithSameSkin *
				GetWaterSkinPowerUnits());
			TotalDamage = WaterDamageToWaterSkin;
			break;
		case ESkin::Plant:
			WaterDamageToPlantSkin = ProjectileDamage - (ProjectileDamage * DamageReductionWithSuferiorSkin *
				GetPlantSkinPowerUnits());
			TotalDamage = WaterDamageToPlantSkin;
			break;
		}
	}
	else
	{
		switch (CharacterOwner->GetCharacterSkin())
		{
			float PlantDamageToFireSkin;
			float PlantDamageToWaterSkin;
			float PlantDamageToPlantSkin;

		case ESkin::Fire:
			PlantDamageToFireSkin = ProjectileDamage - (ProjectileDamage * DamageReductionWithSuferiorSkin *
				GetFireSkinPowerUnits());
			TotalDamage = PlantDamageToFireSkin;
			break;
		case ESkin::Water:
			PlantDamageToWaterSkin = ProjectileDamage + (ProjectileDamage * DamageIncrementWithInferiorSkin *
				GetWaterSkinPowerUnits());
			TotalDamage = PlantDamageToWaterSkin;
			// stunt after 3 hits 
			break;
		case ESkin::Plant:
			PlantDamageToPlantSkin = ProjectileDamage - (ProjectileDamage * DamageReductionWithSameSkin *
				GetPlantSkinPowerUnits());
			TotalDamage = PlantDamageToPlantSkin;
			break;
		}
	}

	return TotalDamage;
}

/*
* IF CHARACTER IS AN ENEMY - Calculate the damage to the enemy based on projectile damage, effects and skins
* @param Projectile: Projectile used against the enemy
*/
float UTC_StatusComponent::CalculateAmmountOfDamageToEnemy(ATC_Projectile* Projectile)
{
	// Save damage of projectile
	float ProjectileDamage = Projectile->GetDamage();

	// Get the player
	ATC_DonutPlayer* DonutPlayer = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!DonutPlayer)
		return Projectile->GetDamage();


	// Get the information of the projectile used from player
	FUProjectileInformation ProjectileUsedInformation;
	DonutPlayer->GetProjectileInformationFromTypeInProjectileComponent(Projectile->GetProjectileType(),
		ProjectileUsedInformation);

	// Save Projectile Stacks and percentage increment
	int ProjectileStacks = ProjectileUsedInformation.GetStacksAccumulated();
	float DamageIncrementPercentagePerStack = ProjectileUsedInformation.GetEffectIncrementDamage();

	// calculate the projectile damage using stacks and increment Damage
	float ProjectileDamageWithEffects = ProjectileDamage + (ProjectileDamage *
		DamageIncrementPercentagePerStack * ProjectileStacks);


	// THE CHARACTER SKIN MUST COME FROM PROJECTILE
	float TotalDamage = 1.f;
	if (Projectile->GetProjectileType() == EProjectile::Fire)
	{
		// Skin of the enemy
		switch (CharacterOwner->GetCharacterSkin())
		{
			case ESkin::Fire:
				// Check the Skin of the player when the projectile was shot
				ProjectileDamageWithEffects += DamageIncrementIfPlayerSkinSameAsProjectile(Projectile, ESkin::Fire);

				TotalDamage = 0.8f * ProjectileDamageWithEffects;
				break;
			case ESkin::Water:
				// Check the Skin of the player when the projectile was shot
				ProjectileDamageWithEffects += DamageIncrementIfPlayerSkinSameAsProjectile(Projectile, ESkin::Fire);

				TotalDamage = 0.5f * ProjectileDamageWithEffects;
				// Use counter attack to this projectile
				// CHECK
				//UseCounterAttack();
				break;
			case ESkin::Plant:
				// Check the Skin of the player when the projectile was shot
				ProjectileDamageWithEffects += DamageIncrementIfPlayerSkinSameAsProjectile(Projectile, ESkin::Fire);

				TotalDamage = 1.2f * ProjectileDamageWithEffects;
				// CHECK
				//MakeDamageThroughTime();
				break;
		}
	}
	else if (Projectile->GetProjectileType() == EProjectile::Water)
	{
		switch (CharacterOwner->GetCharacterSkin())
		{
			case ESkin::Fire:
				// Check the Skin of the player when the projectile was shot
				ProjectileDamageWithEffects += DamageIncrementIfPlayerSkinSameAsProjectile(Projectile, ESkin::Water);

				TotalDamage = 1.2f * ProjectileDamageWithEffects;
				break;
			case ESkin::Water:
				// Check the Skin of the player when the projectile was shot
				ProjectileDamageWithEffects += DamageIncrementIfPlayerSkinSameAsProjectile(Projectile, ESkin::Water);

				TotalDamage = 0.8f * ProjectileDamageWithEffects;
				break;
			case ESkin::Plant:
				// Check the Skin of the player when the projectile was shot
				ProjectileDamageWithEffects += DamageIncrementIfPlayerSkinSameAsProjectile(Projectile, ESkin::Water);

				TotalDamage = 0.5f * ProjectileDamageWithEffects;
				break;
		}
	}
	else if (Projectile->GetProjectileType() == EProjectile::Plant)
	{
		switch (CharacterOwner->GetCharacterSkin())
		{
			case ESkin::Fire:
				// Check the Skin of the player when the projectile was shot
				ProjectileDamageWithEffects += DamageIncrementIfPlayerSkinSameAsProjectile(Projectile, ESkin::Plant);

				TotalDamage = 0.5f * ProjectileDamageWithEffects;
				break;
			case ESkin::Water:
				// Check the Skin of the player when the projectile was shot
				ProjectileDamageWithEffects += DamageIncrementIfPlayerSkinSameAsProjectile(Projectile, ESkin::Plant);

				TotalDamage = 1.2f * ProjectileDamageWithEffects;
				break;
			case ESkin::Plant:
				// Check the Skin of the player when the projectile was shot
				ProjectileDamageWithEffects += DamageIncrementIfPlayerSkinSameAsProjectile(Projectile, ESkin::Plant);

				TotalDamage = 0.8f * ProjectileDamageWithEffects;
				break;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Damage %f"), TotalDamage);
	return FMath::RoundHalfFromZero(TotalDamage);
}

/*
* Check and get the increment of damage, if the player is used the same skin as projectile
* @param Projectile: Projectile used by the player
* @param PlayerSkin: Skin active when the player shot the projectile
* @param DamageMultiplier: Damage to apply depending on the projectile and the skin of the enemy
*/
float UTC_StatusComponent::DamageIncrementIfPlayerSkinSameAsProjectile(ATC_Projectile* Projectile,
	ESkin PlayerSkin)
{
	if (Projectile->GetPlayerSkinWhenFiringProjectile() == PlayerSkin)
	{// Get the player
		ATC_DonutPlayer* DonutPlayer = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
		if (!DonutPlayer)
			return 0;

		// Get the number of units for the specified skin
		int SkinUnits = DonutPlayer->GetPowerUnitsOfType(PlayerSkin);

		float IncrementDamage = Projectile->GetDamage() * DamageIncrementWithInferiorSkin * SkinUnits;

		// Player has the skin of the projectile
		return IncrementDamage;
	}
	return 0.f;
}