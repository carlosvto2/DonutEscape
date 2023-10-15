// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TC_BaseCharacter.generated.h"

class USceneComponent;
class ATC_Projectile;
class UTC_HealthComponent;
class ATC_GameMode;
class ATC_Room;
class ATC_Collectable;

UCLASS()
class ROGUELIKE_API ATC_BaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATC_BaseCharacter();

	UFUNCTION(BlueprintCallable)
		UTC_HealthComponent* GetHealthComponent() { return Health; }
	UFUNCTION(BlueprintCallable)
		UTC_HealthComponent* GetShieldComponent() { return PlayerShield; }

	void HandleDestruction();
	bool GetCanFire() const { return bCanFire; }

	void ActivateEnemy(bool Active) { bCharacterActive = Active; };
	void SetOwnerRoom(ATC_Room* Room) { RoomOwner = Room; }

	// if the character has shield return it
	UTC_HealthComponent* GetShieldIfExist() { return PlayerShield; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UTC_HealthComponent* PlayerShield;

	UPROPERTY(EditAnywhere, Category = "Collectables")
		TArray<TSubclassOf<ATC_Collectable>> Collectables;


	void RotateMesh(FVector LookAtTarget);
	void Fire();
	void DestroyEnemy() { Destroy(); };
	void SetCanFire(bool Value) { bCanFire = Value; }

	ATC_GameMode* DonutGameMode;
	ATC_Room* RoomOwner;

	bool bMultipleShoot = false;


	UPROPERTY(EditAnywhere)
		bool bCharacterActive;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
		TSubclassOf<ATC_Projectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USceneComponent* ProjectileSpawnPoint;

	bool bCanFire = true;

	bool bCollectableAlreadySpawned = false;
	void SpawnCollectable();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UTC_HealthComponent* Health;

};
