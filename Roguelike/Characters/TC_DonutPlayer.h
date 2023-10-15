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
class UCharacterMovementComponent;
class ATC_Projectile;
class ATC_Collectable;

UCLASS()
class ROGUELIKE_API ATC_DonutPlayer : public ATC_BaseCharacter
{
	GENERATED_BODY()
	
public:
	ATC_DonutPlayer();

	UFUNCTION(BlueprintCallable)
		float GetNumberOfBombs() { return NumberOfBombs; }

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HandleDestruction();

	APlayerController* GetDonutPlayerController() const { return DonutPlayerController; };



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool CheckIfOverlappingCollectables(TArray<AActor*>& OverlappedActors);
	void PickupCollector();
	void CastToPossibleCollectables(ATC_Collectable* Collectable);

public:
	void RotateCharacter(float RotationDegrees);

	float GetOverheat() const { return Overheat; }
	float GetMaxOverheat() const { return MaxOverheat; }
	bool GetIfPlayerInOverheat() const { return bInOverheat; }
	void IncrementOverheat();
	void DecreaseOverheat();
	void OverheatReaction();
	void RecoverFromOverheat();

	void IncrementBombs(int Number);
	void ReduceBombs();

	void MultipleShoot();

	void IncrementHealth(float HealthIncrementation);
	void IncrementShield(float ShieldIncrementation);

private:
	UPROPERTY(EditAnywhere, Category = "Component")
		USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category = "Component")
		UCameraComponent* Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USphereComponent* SphereCollector;

	/** MappingContext */
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

	UPROPERTY(EditAnywhere, Category = "Overheat")
		float OverheatIncrementation = 10.f;
	UPROPERTY(EditAnywhere, Category = "Overheat")
		float OverheatDecrement = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
		TSubclassOf<ATC_Projectile> BombClass;

	void Move(const FInputActionValue& Value);
	void RotateCamera();

	void ThrowBomb();

	APlayerController* DonutPlayerController;
	UCharacterMovementComponent* MovementComponent;

	// if the player has the key of the dungeon
	bool KeyPicked = false;

	// Overheat when throw balls
	float Overheat = 0.f;
	float MaxOverheat = 100.f;
	bool bInOverheat = false;

	// Bombs
	int NumberOfBombs = 0;

	// camera
	bool bRotateCamera = false;
	FRotator SpringArmActualRotation;
	float CameraSpeedRotation = 1.5f;
	FRotator CameraRotationAngle = FRotator(0.f, 0.f, 0.f);
	FRotator FinalCameraRotationAngle = FRotator(0.f, 0.f, 0.f);
	bool bCameraRotating = false;
};
