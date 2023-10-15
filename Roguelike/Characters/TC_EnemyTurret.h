// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TC_BaseCharacter.h"
#include "TC_EnemyTurret.generated.h"

class ATC_DonutPlayer;
class AAIController;
class UStaticMeshComponent;
class USoundWave;

UCLASS()
class ROGUELIKE_API ATC_EnemyTurret : public ATC_BaseCharacter
{
	GENERATED_BODY()
	
public:
	ATC_EnemyTurret();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HandleDestruction();

	UFUNCTION(BlueprintImplementableEvent)
		void TurretExplosionEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* BaseTurret;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* TurningTurret;

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundWave* ExplosionSound;

	UPROPERTY(EditAnywhere, Category = "DestroyDelay")
		float FirstFireDelay = 2.f;
	UPROPERTY(EditAnywhere, Category = "DestroyDelay")
		float DestroyDelay = 2.f;
	UPROPERTY(EditAnywhere, Category = "DestroyDelay")
		float FireRate = 2.0f;

	AAIController* EnemyTurretController;
	ATC_DonutPlayer* Donut;
	FTimerHandle FireRateTimerHandle;
};
