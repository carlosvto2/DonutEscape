// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TC_BaseCharacter.h"
#include "TC_Boss.generated.h"

class ATC_DonutPlayer;
class AAIController;
class UStaticMeshComponent;
class USoundWave;

UCLASS()
class ROGUELIKE_API ATC_Boss : public ATC_BaseCharacter
{
	GENERATED_BODY()

public:
	ATC_Boss(const FObjectInitializer& OI);

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* TurningSecondTurret;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* TurningThirdTurret;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USceneComponent* SecondProjectileSpawnPoint;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USceneComponent* ThirdProjectileSpawnPoint;

	UPROPERTY(EditAnywhere, Category = "Combat")
		FVector ProjectileScale = FVector(1.0f, 1.0f, 1.0f);

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundWave* ExplosionSound;

	UPROPERTY(EditAnywhere, Category = "DestroyDelay")
		float FirstFireDelay = 2.f;
	UPROPERTY(EditAnywhere, Category = "DestroyDelay")
		float DestroyDelay = 2.f;
	UPROPERTY(EditAnywhere, Category = "DestroyDelay")
		float FireRate = 2.0f;

	void Fire();
	void SpawnCollectable();

	AAIController* EnemyTurretController;
	ATC_DonutPlayer* Donut;
	FTimerHandle FireRateTimerHandle;
};
