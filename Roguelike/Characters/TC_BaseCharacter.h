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
class USphereComponent;
class UTC_StatusComponent;
class UTC_WidgetComponent;


// Projectile the character is using
UENUM(BlueprintType)
enum class EProjectile : uint8
{
	Fire UMETA(DisplayName = "Fire Projectile"),
	Water UMETA(DisplayName = "Water Projectile"),
	Plant UMETA(DisplayName = "Plant Projectile")
};

// Skin the character is using
UENUM(BlueprintType)
enum class ESkin : uint8
{
	None UMETA(DisplayName = "No Skin"),
	Fire UMETA(DisplayName = "Fire Skin"),
	Water UMETA(DisplayName = "Water Skin"),
	Plant UMETA(DisplayName = "Plant Skin")
};


UCLASS()
class ROGUELIKE_API ATC_BaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
		TSubclassOf<ATC_Projectile> FireProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
		TSubclassOf<ATC_Projectile> WaterProjectileClass;
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
		TSubclassOf<ATC_Projectile> PlantProjectileClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USceneComponent* ProjectileSpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetPosition")
		USceneComponent* WidgetPosition;

	// Sets default values for this character's properties
	ATC_BaseCharacter(const FObjectInitializer& OI);

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

	// Get the status component of the character
	UTC_StatusComponent* GetStatusComponent() { return StatusComponent; }

	// Skin - Projectile
	void CalculateAmmountOfDamageToCharacter(ATC_Projectile* Projectile, float& TotalDamage);

	ESkin GetCharacterSkin() const { return CharacterSkin; }

	void UseCounterAttack();
	virtual void CounterWaterAttackToFireProjectile() {};
	virtual void CounterFireAttackToPlantProjectile() {};
	virtual void CounterPlantAttackToWaterProjectile() {};

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


	/* SKIN MATERIALS FIRE */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* FireSkin1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* FireSkin2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* FireSkin3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* FireSkin4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* FireSkin5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* FireSkin6;
	/* SKIN MATERIALS WATER */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Water Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* WaterSkin1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Water Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* WaterSkin2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Water Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* WaterSkin3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Water Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* WaterSkin4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Water Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* WaterSkin5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Water Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* WaterSkin6;
	/* SKIN MATERIALS PLANT */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Plant Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* PlantSkin1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Plant Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* PlantSkin2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Plant Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* PlantSkin3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Plant Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* PlantSkin4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Plant Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* PlantSkin5;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Plant Skin", meta = (AllowPrivateAccess = "true"))
		UMaterialInstance* PlantSkin6;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USphereComponent* SphereToCheckPlayerDistance;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UTC_HealthComponent* PlayerShield;


	/* This will control the status and effects on the character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
		UTC_StatusComponent* StatusComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Collectables")
		TArray<TSubclassOf<ATC_Collectable>> Collectables;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Skin")
		ESkin CharacterSkin = ESkin::None;

	void RotateMesh(FVector LookAtTarget);
	void Fire();
	void DestroyEnemy() { Destroy(); };
	void SetCanFire(bool Value) { bCanFire = Value; }

	ATC_GameMode* DonutGameMode;
	ATC_Room* RoomOwner;

	bool bMultipleShoot = false;



	UPROPERTY(EditAnywhere)
		bool bCharacterActive;


	UFUNCTION()
		virtual void OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
		void SetStatusComponent(UTC_StatusComponent* Component);

	void SpawnCollectable();


	bool bCanFire = true;
	// grab
	UPrimitiveComponent* ItemGrabbed;

	bool bCollectableAlreadySpawned = false;

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UTC_HealthComponent* Health;

	

	/* PROJECTILE SIDE EFFECTS */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Side Effects", meta = (AllowPrivateAccess = "true"))
		int BurningTimeMax = 5;
	int BurningTimeCounter = 0;
	FTimerHandle FireBurnDamageTimer;
	void MakeDamageThroughTime();
	void MakeBurningDamage(float Damage);
};
