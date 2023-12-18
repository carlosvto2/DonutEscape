// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TC_SideEffect.generated.h"

class ATC_GameMode;

// Effect type
UENUM(BlueprintType)
enum class EEffectType : uint8
{
	Stack UMETA(DisplayName = "Stack Effect")
};

UCLASS()
class ROGUELIKE_API ATC_SideEffect : public AActor
{
	GENERATED_BODY()
	
public:
	ATC_SideEffect();

	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	EEffectType GetEffectType() const { return EffectType; }
	UTexture2D* GetEffectTexture() const { return EffectTexture; }
	int GetStacks() const { return Stacks; }
	float GetDamagePercentage() const { return DamagePercentage; }
	void IncrementStacks();

	FUProjectileInformation GetProjectileAssignedInfo() const{ return ProjectileAssignedInfo; }
	void SetProjectileAssigned(FUProjectileInformation Projectile) { ProjectileAssignedInfo = Projectile; }

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texture", meta = (AllowPrivateAccess = "true"))
		EEffectType EffectType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Texture", meta = (AllowPrivateAccess = "true"))
		UTexture2D* EffectTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
		float EffectDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
		float DamagePercentage;

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
		bool IncrementDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (EditCondition = "IncrementDamage"))
		int IncrementFireDamagePercentage;
	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (EditCondition = "IncrementDamage"))
		int IncrementWaterDamagePercentage;
	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (EditCondition = "IncrementDamage"))
		int IncrementPlantDamagePercentage;*/


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
		bool ReceiveDamage;

	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta = (EditCondition = "ReceiveDamage"))
		int PeriodicallyDamaged;

	void DeactivateEffect();

	int Stacks = 0;
	float DurationLeft;
	ATC_GameMode* DonutGameMode;

	FUProjectileInformation ProjectileAssignedInfo;
};
