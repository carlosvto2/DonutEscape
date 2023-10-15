// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/Array.h"

// Sets default values
ATC_Projectile::ATC_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Projectile Mesh"));
	RootComponent = ProjectileMesh;

	// create the movement component and set the speed
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 1300.0f;
	ProjectileMovementComponent->MaxSpeed = 1300.0f;
}

// Called when the game starts or when spawned
void ATC_Projectile::BeginPlay()
{
	Super::BeginPlay();

	if (!FireBallSound)
		return;
	UGameplayStatics::PlaySound2D(this, FireBallSound, 1.0f, ExplosionPitchMultiplier);
	
	// if the projectile needs to hit
	if (NeedToHit)
	{
		// When something hits the projectile, it will call the OnHit function - DELEGATE
		ProjectileMesh->OnComponentHit.AddDynamic(this, &ATC_Projectile::OnHit);
	}
	else
	{
		// Timer to enable the explosion of the bomb
		FTimerHandle ExplosionBombTimer;
		GetWorldTimerManager().SetTimer(ExplosionBombTimer, this, &ATC_Projectile::ProjectileExplosion, ExplosionDelay, false);
		BombExplosionEffect();
	}
}

// Called every frame
void ATC_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATC_Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor && GetOwner())
	{
		AController* ControllerInstigator = GetOwner()->GetInstigatorController();
		if (ControllerInstigator)
		{
			// if the otheractor exist, is not this actor, and not the owner
			if (OtherActor && OtherActor != this && OtherActor != GetOwner())
			{
				// apply the damage to the other actor
				UGameplayStatics::ApplyDamage(OtherActor, Damage, ControllerInstigator, this, UDamageType::StaticClass());
			}
		}
	}	

	// Destroy projectile
	Destroy();
}

void ATC_Projectile::ProjectileExplosion()
{
	if (GetOwner())
	{
		AController* ControllerInstigator = GetOwner()->GetInstigatorController();
		if (ControllerInstigator)
		{
			TArray<AActor*> IgnoreActors;
			bool a = UGameplayStatics::ApplyRadialDamage(this, Damage,
				GetActorLocation(), ExplosionRadius, UDamageType::StaticClass(),
				IgnoreActors, this, ControllerInstigator);

		}

		if (ExplosionSound)
		{
			UGameplayStatics::PlaySound2D(this, ExplosionSound, 0.4f, 4.0f);
		}
	}
	Destroy();
}
