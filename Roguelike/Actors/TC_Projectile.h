// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TC_Projectile.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class USoundWave;
class ATC_DonutPlayer;
class ATC_SideEffect;

UCLASS()
class ROGUELIKE_API ATC_Projectile : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(EditAnywhere, Category = "Combat")
		bool MovingProjectile = true;
	UPROPERTY(EditAnywhere, Category = "Combat")
		bool ThrowingFromHand = false;

	// Sets default values for this actor's properties
	ATC_Projectile();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
		void BombExplosionEffect();


	float GetDamage() const { return Damage; }
	EProjectile GetProjectileType() const { return ProjectileType; }
	float GetProjectileCadency() const { return CadencyProjectile; }
	int GetMaxProjectilesToUse() const { return MaxProjectilesToUse; }
	TSubclassOf<ATC_SideEffect> GetEffectToApplyAfterRecover() const { return EffectToApplyAfterRecover; }

	ESkin GetPlayerSkinWhenFiringProjectile() const { return PlayerSkinWhenFire; }
	void SetPlayerSkinWhenFiringProjectile(ESkin PlayerSkin) { PlayerSkinWhenFire = PlayerSkin; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Combat")
		UStaticMeshComponent* ProjectileMesh;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		EProjectile ProjectileType;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<ATC_SideEffect> EffectToApplyAfterRecover;

	UPROPERTY(EditAnywhere, Category = "Combat")
		bool ProjectileStuntCharacter = false;

	UPROPERTY(EditAnywhere, Category = "Combat")
		bool DestroyAfterHit = true;

	UPROPERTY(EditAnywhere, Category = "Combat")
		FRotator ProjectileRotationAfterHit;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Movement", meta = (AllowPrivateAccess = "true"))
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

	UFUNCTION() // Delegate
		void OverlapStaticProjectile(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION() // Delegate
		void EndOverlapStaticProjectile(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ProjectileExplosion();

	void CheckIfCharactersOverlapping();
	void MakeDamage(AActor* ActorToDamage, float TotalDamage);
	void DestroyProjectile();
	void ProjectileBehaviorAfterHit();
	UFUNCTION() // Delegate
		void RecoverProjectile(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, Category = "Projectile")
		float Damage = 25.0f;
	UPROPERTY(EditAnywhere)
		float ExplosionRadius = 500.0f;
	UPROPERTY(EditAnywhere)
		float ExplosionDelay = 2.0f;
	UPROPERTY(EditAnywhere, Category = "Projectile")
		float DestroyProjectileDelay = 2.0f;

	// Projectile use values
	UPROPERTY(EditAnywhere, Category = "Projectile")
		float CadencyProjectile = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Projectile")
		int MaxProjectilesToUse = 10;

	UPROPERTY(EditAnywhere, Category = "Projectile")
		float MaxTimeForProjectileOverlapDamage = 0.5f;
	UPROPERTY(EditAnywhere, Category = "Projectile")
		FVector ProjectileVelocityAfterHit = FVector(0.f, 0.f, 20.f);

	float TimeGoneForProjectileOverlapDamage = 0.f;

	// Projectile type
	TArray<AActor*> OverlappingActors;

	bool AlreadyBounced = false;

	bool ProjectileHitEnemy = false;



	ATC_DonutPlayer* DonutPlayer;
	ESkin PlayerSkinWhenFire;
};
