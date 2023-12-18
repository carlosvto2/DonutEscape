// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "../Characters/TC_DonutPlayer.h"
#include "TC_HandleProjectile.generated.h"

class ATC_GameMode;
class ATC_DonutPlayer;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROGUELIKE_API UTC_HandleProjectile : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTC_HandleProjectile();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetProjectileInformation(EProjectile ProjectileType);

	void CheckProjectileRange(FHitResult HitCameraMouse);

	void GetProjectileInformationFromType(EProjectile ProjectileType, FUProjectileInformation& ProjInformation);

	void SetProjectileOfFire();
	void SetProjectileOfWater();
	void SetProjectileOfPlant();
	void FireProjectile();
	void ReloadProjectiles(EProjectile ProjectileTypeToReload);

	void UpdateProjectileStackInformation(FUProjectileInformation ProjectileToUpdate, int Stacks,
		float IncrementDamagePercentage);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	
private:
	ATC_GameMode* DonutGameMode;
	ATC_DonutPlayer* DonutPlayer;

	// Projectile
	FHitResult MousePosition;
	USceneComponent* ProjectileSpawnPoint;
	bool ProjectileInRange;

	float MinDistanceForStaticProjectiles = 800.f;

	bool MouseInRange();

	void ProjectileOfFire();

	void SpawnProjectile();

	void SaveProjectilesInformation();

	FVector ProjectileSpawnPosition;
	FRotator ProjectileSpawnRotation;
	EProjectile TypeUsingProjectile;

	// Projectiles struct
	FUProjectileInformation ProjectileUsing;
	FUProjectileInformation FireProjInformation;
	FUProjectileInformation WaterProjInformation;
	FUProjectileInformation PlantProjInformation;

};
