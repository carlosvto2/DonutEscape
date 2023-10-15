// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TC_BasePawn.h"
#include "TC_Donut.generated.h"

class USpringArmComponent;
class UCameraComponent;
class APlayerController;

UCLASS()
class ROGUELIKE_API ATC_Donut : public ATC_BasePawn
{
	GENERATED_BODY()

public:
	ATC_Donut();

	APlayerController* GetPlayerController() const { return DonutController; }

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = "Component")
		USpringArmComponent* SpringArm;
	UPROPERTY(VisibleAnywhere, Category = "Component")
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, Category = "Movement")
		float Speed = 200.0f;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float TurnRate = 45.0f;

	APlayerController* DonutController;

	void Move(float Value);
	void Turn(float Value);
};
