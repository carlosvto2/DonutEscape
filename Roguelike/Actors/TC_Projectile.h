// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TC_Projectile.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class USoundWave;

UCLASS()
class ROGUELIKE_API ATC_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATC_Projectile();

	UFUNCTION(BlueprintImplementableEvent)
		void BombExplosionEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
		UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
		UProjectileMovementComponent* ProjectileMovementComponent;
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundWave* FireBallSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundWave* ExplosionSound;

	UPROPERTY(EditAnywhere, Category = "Sound")
		float ExplosionPitchMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Hit")
		bool NeedToHit = true;

	UFUNCTION() // Delegate
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	void ProjectileExplosion();

	UPROPERTY(EditAnywhere)
		float Damage = 25.0f;
	UPROPERTY(EditAnywhere)
		float ExplosionRadius = 500.0f;
	UPROPERTY(EditAnywhere)
		float ExplosionDelay = 2.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
