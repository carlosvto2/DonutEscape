// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TC_BaseCharacter.h"
#include "InputActionValue.h"
#include "TC_DonutPlayer.generated.h"

class USphereComponent;
class USpringArmComponent;
class UCameraComponent;
class APlayerController;
class UInputMappingContext;
class UInputAction;
class ATC_Projectile;
class ATC_Collectable;
class UTC_AnimInstance;
class ATC_DonutPlayerController;
class UTC_HandleProjectile;
class ATC_SideEffect;


// Struct to save all the information of each projectile
USTRUCT(BlueprintType)
struct FUProjectileInformation
{
	GENERATED_USTRUCT_BODY()

	TSubclassOf<ATC_Projectile> ProjectileClass;
	EProjectile ProjectileType;
	float CadencyProjectile;
	int MaxProjectilesToUse;
	int ProjectilesLeft;
	bool MovingProjectile;
	bool ThrowingFromHand;
	// Side effect
	TSubclassOf<ATC_SideEffect> SideEffectAfterRecover;
	int StacksAccumulated;
	float EffectIncrementDamage;

	// GET
	TSubclassOf<ATC_Projectile> GetProjectileClass() { return ProjectileClass; }
	EProjectile GetProjectileType() { return ProjectileType; }
	float GetCadencyProjectile() { return CadencyProjectile; }
	int GetMaxProjectilesToUse() { return MaxProjectilesToUse; }
	int GetProjectilesLeft() { return ProjectilesLeft; }
	bool GetIfMovingProjectile() { return MovingProjectile; }
	bool GetIfThrowingFromHand() { return ThrowingFromHand; }
	// Side Effects
	TSubclassOf<ATC_SideEffect> GetSideEffectAfterRecover() { return SideEffectAfterRecover; }
	int GetStacksAccumulated() { return StacksAccumulated; }
	float GetEffectIncrementDamage() { return EffectIncrementDamage; }

	// SET
	void SetProjectileClass(TSubclassOf<ATC_Projectile> Projectile) { ProjectileClass = Projectile; }
	void SetProjectileType(EProjectile Projectile) { ProjectileType = Projectile; }
	void SetCadencyProjectile(float Cadency) { CadencyProjectile = Cadency; }
	void SetMaxProjectilesToUse(int MaxProjectiles) { MaxProjectilesToUse = MaxProjectiles; }
	void SetProjectilesLeft(int ProjLeft) { ProjectilesLeft = ProjLeft; }
	void SetIMovingProjectile(bool IfMoving) { MovingProjectile = IfMoving; }
	void SetIfThrowingFromHand(bool IfThrowing) { ThrowingFromHand = IfThrowing; }
	// Side Effects
	void SetSideEffectAfterRecover(TSubclassOf<ATC_SideEffect> SideEffect) { SideEffectAfterRecover = SideEffect; }
	void SetStacksAccumulated(int Stacks) { StacksAccumulated = Stacks; }
	void SetEffectIncrementDamage(float IncrementPercentage) { EffectIncrementDamage = IncrementPercentage; }

	// Initialize variables
	FUProjectileInformation()
	{
		ProjectileClass = nullptr;
		ProjectileType = EProjectile::Fire;
		CadencyProjectile = 0.f;
		MaxProjectilesToUse = 0;
		ProjectilesLeft = 0;
		MovingProjectile = true;
		ThrowingFromHand = false;
		// Side Effects
		SideEffectAfterRecover = nullptr;
		StacksAccumulated = 0;
		EffectIncrementDamage = 0.f;
	}
};

// Struct to save the information of a power
USTRUCT(BlueprintType)
struct FUSkinInformation
{
	GENERATED_USTRUCT_BODY()

	ESkin SkinType;

	// GET
	ESkin GetSkinType() { return SkinType; }

	// SET
	void SetSkinType(ESkin Skin) { SkinType = Skin; }

	// Initialize variables
	FUSkinInformation()
	{
		SkinType = ESkin::Fire;
	}
};


UCLASS()
class ROGUELIKE_API ATC_DonutPlayer : public ATC_BaseCharacter
{
	GENERATED_BODY()
	
public:
	ATC_DonutPlayer(const FObjectInitializer& OI);

	UPROPERTY(EditAnywhere, Category = "Aim")
		FRotator AimOffset;

	UFUNCTION(BlueprintCallable)
		float GetNumberOfBombs() { return NumberOfBombs; }

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HandleDestruction();

	ATC_DonutPlayerController* GetDonutPlayerController() const { return DonutPlayerController; };

	void ReloadProjectilesAfterOverlap(EProjectile ProjectileType, bool IfProjectileHitEnemy);

	void GetProjectileInformationFromTypeInProjectileComponent(EProjectile ProjectileType,
		FUProjectileInformation& ProjInformation);

	int GetPowerUnitsOfType(ESkin SkinType);

protected:
	// Called when the game starts or when spawned
	void SetPrincipalVariableValues();
	virtual void BeginPlay() override;

public:
	USphereComponent* GetSphereGrabber() { return SphereGrabber; }
	UCameraComponent* GetPlayerCamera() { return Camera; }

	void RotateCharacter(float RotationDegrees);

	void IncrementBombs(int Number);
	void ReduceBombs();

	void MultipleShoot();

	void IncrementHealth(float HealthIncrementation);
	void IncrementShield(float ShieldIncrementation);

	void HandleThrowItem();

	template< class UserClass >
	void CreateGenericTimer(UserClass* InObj, typename FTimerDelegate::TMethodPtr<UserClass> InTimerMethod,	
		float Delay);

	void TimerToReloadProjectiles(float Delay, EProjectile ProjectileTypeToLoad);

	void UpdateProjectileStacks(FUProjectileInformation ProjectileRecovered,
		int ProjectileStacks, float ProjectilePercentageDamageIncrement);

	void ExecuteAnimation(FString AnimationToExecute);
	void ExecuteAnimMontage(FString AnimationToExecute);

	UFUNCTION(BlueprintCallable)
		void BuyItemInShop(FText ItemBought, int PowerUnitsToPay, int MoneyToPay);

private:
	/////// COMPONENTS
	UPROPERTY(EditAnywhere, Category = "Component")
		USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category = "Component")
		UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USphereComponent* SphereCollector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USphereComponent* SphereGrabber;
	// component to handle the projectiles
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UTC_HandleProjectile* HandleProjectileComponent;
	/////// END COMPONENTS

	/** MAPPING CONTEXT */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputMappingContext* MappingContext;
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* IMove;
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* IFire;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* IBomb;
	/** Pick collectable Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* IPick;
	/** Defend with item */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* IDefend;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* IFireProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* IWaterProjectile;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UInputAction* IPlantProjectile;
	/** END MAPPING CONTEXT */

	

	/* Animation blueprint */
	UTC_AnimInstance* AnimInst;
	UPROPERTY(EditAnywhere, Category = Animation)
		UAnimMontage* DefendAnimMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
		UAnimMontage* ThrowAnimMontage;
	/* End Animation blueprint */

	// GRAB THROW OBJECT
	UPROPERTY(EditAnywhere, Category = "Overheat")
		float OverheatIncrementation = 10.f;
	UPROPERTY(EditAnywhere, Category = "Overheat")
		float OverheatDecrement = 10.f;
	// grab
	UPROPERTY(EditAnywhere, Category = "Grab")
		float GrabItemDelay = 1.0f;
	// throw
	UPROPERTY(EditAnywhere, Category = "Throw")
		float ThrowItemDelay = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Throw")
		FRotator ThrowDeviation = FRotator(0.f, -5.f, 0.f);
	// GRAB THROW OBJECT
	

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
		TSubclassOf<ATC_Projectile> BombClass;

	// FUNCTIONS
	void Move(const FInputActionValue& Value);
	void RotateCamera();

	void ThrowBomb();

	void CollectablesHideWidgets();
	bool CheckIfOverlappingComponents();
	void PickupCollector();
	void CastToPossibleCollectables(ATC_Collectable* Collectable);

	void GrabItem(UPrimitiveComponent* ComponentToGrab);
	void AttachItem();
	void ThrowItem();
	void DefendWithItem();
	void StopDefense();

	void Fire();
	// END FUNCTIONS
	TSet<UPrimitiveComponent*> OverlappedComponents;

	ATC_DonutPlayerController* DonutPlayerController;

	// if the player has the key of the dungeon
	bool KeyPicked = false;

	// Overheat when throw balls
	float Overheat = 0.f;
	float MaxOverheat = 100.f;
	bool bInOverheat = false;

	// Bombs
	int NumberOfBombs = 0;

	// camera
	FHitResult HitCameraMouse;
	bool bRotateCamera = false;
	FRotator SpringArmActualRotation;
	float CameraSpeedRotation = 1.5f;
	FRotator CameraRotationAngle = FRotator(0.f, 0.f, 0.f);
	FRotator FinalCameraRotationAngle = FRotator(0.f, 0.f, 0.f);
	bool bCameraRotating = false;

};
