// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TC_StatusComponent.generated.h"

class ATC_GameMode;
class ATC_SideEffect;
class ATC_Projectile;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROGUELIKE_API UTC_StatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	// Sets default values for this component's properties
	UTC_StatusComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CheckIfSideEffectExists(TSubclassOf<ATC_SideEffect> SideEffectToApply, ATC_SideEffect*& SideEffect);

	void AddSideEffect(FUProjectileInformation ProjectileRecovered);
	void RemoveSideEffect(ATC_SideEffect* SideEffectToRemove);


	// SKIN POWER FUNCTIONS
	int GetFireSkinPowerUnits() const { return FireSkinPowerUnits; }
	int GetWaterSkinPowerUnits() const { return WaterSkinPowerUnits; }
	int GetPlantSkinPowerUnits() const { return PlantSkinPowerUnits; }

	void SetFireSkinPowerUnits(int PowerIncrement) { FireSkinPowerUnits += PowerIncrement; }
	void SetWaterSkinPowerUnits(int PowerIncrement) { WaterSkinPowerUnits += PowerIncrement; }
	void SetPlantSkinPowerUnits(int PowerIncrement) { PlantSkinPowerUnits += PowerIncrement; }
	// END SKIN POWER FUNCTIONS

	float CalculateAmmountOfDamageToPlayer(ATC_Projectile* Projectile);
	float CalculateAmmountOfDamageToEnemy(ATC_Projectile* Projectile);
	float DamageIncrementIfPlayerSkinSameAsProjectile(ATC_Projectile* Projectile, ESkin PlayerSkin);

	/* POWER UNITS */
	void InitializePlayerSkinsPowerUnits();
	void UpdatePowerUnitsInPossession(int Units);

	void UpdateItemsPlayer(FText ItemBought, int PowerUnitsToPay, int MoneyToPay);

	UFUNCTION(BlueprintCallable)
		int GetCharacterPowerUnits() const { return PowerUnits; }
	/* END POWER UNITS */


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	/* DAMAGE INCREMENT/DECREASE DEPENDING ON SKIN AND PROJECTILE TYPES */
	// This will be applied for every unit of power against the player
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Damage", meta = (AllowPrivateAccess = "true"))
		float DamageIncrementWithInferiorSkin = 0.1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Damage", meta = (AllowPrivateAccess = "true"))
		float DamageReductionWithSuferiorSkin = 0.01;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Damage", meta = (AllowPrivateAccess = "true"))
		float DamageReductionWithSameSkin = 0.005;
	/* END DAMAGE INCREMENT/DECREASE DEPENDING ON SKIN AND PROJECTILE TYPES */

	// SKIN POWER UNITS
	UPROPERTY(EditAnywhere, Category = "Power Units")
		int FireSkinPowerUnits = 1;
	UPROPERTY(EditAnywhere, Category = "Power Units")
		int WaterSkinPowerUnits = 1;
	UPROPERTY(EditAnywhere, Category = "Power Units")
		int PlantSkinPowerUnits = 1;
	UPROPERTY(EditAnywhere, Category = "Power Units")
		int MaxFireSkinPowerUnits = 50;
	UPROPERTY(EditAnywhere, Category = "Power Units")
		int MaxWaterSkinPowerUnits = 50;
	UPROPERTY(EditAnywhere, Category = "Power Units")
		int MaxPlantSkinPowerUnits = 50;

	void AddSideEffectStacks(FUProjectileInformation ProjectileRecovered);
		
	TArray<ATC_SideEffect*> CharacterEffects;

	ATC_GameMode* DonutGameMode;

	ATC_BaseCharacter* CharacterOwner;

	int PowerUnits = 0;
	
};
