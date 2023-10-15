// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TC_BaseCharacter.h"
#include "TC_ChaseEnemy.generated.h"

class AAIController;
class ATC_DonutPlayer;
class USoundWave;

UCLASS()
class ROGUELIKE_API ATC_ChaseEnemy : public ATC_BaseCharacter
{
	GENERATED_BODY()

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void HandleDestruction();

	UFUNCTION(BlueprintImplementableEvent)
		void EnemyExplosionEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundWave* ExplosionSound;

	UPROPERTY(EditAnywhere, Category = "DestroyDelay")
		float DestroyDelay = 1.f;

	UFUNCTION() // Delegate
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ChasePlayer();

	AAIController* EnemyChaseController;
	ATC_DonutPlayer* Donut;
	UPROPERTY(EditAnywhere)
		float Damage = 25.0f;
};
