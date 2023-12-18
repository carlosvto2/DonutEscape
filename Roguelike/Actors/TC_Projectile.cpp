// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_Projectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Containers/Array.h"
#include "../Characters/TC_DonutPlayer.h"
#include "../Characters/TC_ChaseEnemy.h"
#include "../Characters/TC_EnemyTurret.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATC_Projectile::ATC_Projectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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
	AlreadyBounced = false;

	// Get a reference to the players pawn
	DonutPlayer = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));

	if (!FireBallSound)
		return;
	UGameplayStatics::PlaySound2D(this, FireBallSound, 1.0f, ExplosionPitchMultiplier);
	
	// if the projectile needs to hit
	if (!NeedToHit)
	{
		// Timer to enable the explosion of the bomb
		FTimerHandle ExplosionBombTimer;
		GetWorldTimerManager().SetTimer(ExplosionBombTimer, this, &ATC_Projectile::ProjectileExplosion, ExplosionDelay, false);
		BombExplosionEffect();
	}


	// if the projectile moves
	if (MovingProjectile)
	{
		// When something hits the projectile, it will call the OnHit function - DELEGATE
		ProjectileMesh->OnComponentHit.AddDynamic(this, &ATC_Projectile::OnHit);
		// if overlap moving projectile, recover it
		ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &ATC_Projectile::RecoverProjectile);
	}
	else
	{
		// When the projectile does not move, check if any character overlapps
		ProjectileMesh->OnComponentBeginOverlap.AddDynamic(this, &ATC_Projectile::OverlapStaticProjectile);
		ProjectileMesh->OnComponentEndOverlap.AddDynamic(this, &ATC_Projectile::EndOverlapStaticProjectile);

		// Timer to check if any character overlapping at the beginning (does not work in begin play)
		FTimerHandle CharacterOverlapping;
		GetWorldTimerManager().SetTimer(CharacterOverlapping, this, &ATC_Projectile::CheckIfCharactersOverlapping, 0.1f, false);

		// Timer to destroy the static projectile
		FTimerHandle DestroyProjectileTimer;
		GetWorldTimerManager().SetTimer(DestroyProjectileTimer, this, &ATC_Projectile::DestroyProjectile, DestroyProjectileDelay, false);
	}
}

void ATC_Projectile::CheckIfCharactersOverlapping()
{
	// Check if any character overlapping
	GetOverlappingActors(OverlappingActors, AActor::StaticClass());
}

// Called every frame
void ATC_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Projectile damage every 0.5 seconds
	if (TimeGoneForProjectileOverlapDamage <= 0)
	{
		// Apply damage if the enemy is overlapping the water projectile
		if (OverlappingActors.Num() > 0)
		{
			for (AActor* OverlappedActor : OverlappingActors)
			{
				ATC_BaseCharacter* OverlappedCharacter = Cast<ATC_BaseCharacter>(OverlappedActor);
				// check the skin type of PLAYER and apply the side effects
				float TotalDamage;
				OverlappedCharacter->CalculateAmmountOfDamageToCharacter(this, TotalDamage);
				MakeDamage(OverlappedActor, TotalDamage);
			}
		}
		TimeGoneForProjectileOverlapDamage = MaxTimeForProjectileOverlapDamage;
	}
	else
	{
		TimeGoneForProjectileOverlapDamage -= DeltaTime;
	}

	
}


/*
* Call this function if character overlapps
*/
void ATC_Projectile::OverlapStaticProjectile(UPrimitiveComponent* OverlappedComp,
	AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult& SweepResult)
{
	// Other actor that overlapped the projectile
	if (Other)
	{
		OverlappingActors.Add(Other);
	}
}

/*
* Call this function if character ends overlapping
*/
void ATC_Projectile::EndOverlapStaticProjectile(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlappingActors.Remove(OtherActor);
}


void ATC_Projectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	float TotalDamage = 1.f;
	if (OtherActor && OtherActor != GetOwner())
	{
		ATC_BaseCharacter* HitCharacter = Cast<ATC_BaseCharacter>(OtherActor);
		if (HitCharacter)
		{
			if (OtherActor != DonutPlayer)
			{
				// Set that the projectile hit the enemy
				ProjectileHitEnemy = true;
			}


			// Calculate the total damage to the character
			HitCharacter->CalculateAmmountOfDamageToCharacter(this, TotalDamage);

			MakeDamage(OtherActor, TotalDamage);
		}

		// Destroy projectile
		if (DestroyAfterHit)
		{
			DestroyProjectile();
		}
		else
		{
			ProjectileBehaviorAfterHit();
		}
	}
}

void ATC_Projectile::ProjectileExplosion()
{
	if (GetOwner())
	{
		AController* ControllerInstigator = GetOwner()->GetInstigatorController();
		if (ControllerInstigator)
		{
			TArray<AActor*> IgnoreActors;
			UGameplayStatics::ApplyRadialDamage(this, Damage,
				GetActorLocation(), ExplosionRadius, UDamageType::StaticClass(),
				IgnoreActors, this, ControllerInstigator);

		}

		if (ExplosionSound)
		{
			UGameplayStatics::PlaySound2D(this, ExplosionSound, 0.4f, 4.0f);
		}
	}
	
	if(DestroyAfterHit)
		DestroyProjectile();
}


/*
* Make damage to an specific actor
*/
void ATC_Projectile::MakeDamage(AActor* ActorToDamage, float TotalDamage)
{
	// if the otheractor exist, is not this actor, and not the owner
	if (ActorToDamage && GetOwner())
	{
		AController* ControllerInstigator = GetOwner()->GetInstigatorController();
		if (ControllerInstigator)
		{
			// apply the damage to the other actor
			UGameplayStatics::ApplyDamage(ActorToDamage, TotalDamage, ControllerInstigator, this, UDamageType::StaticClass());
			
		}
	}
}

void ATC_Projectile::DestroyProjectile()
{
	Destroy();
}

/*
* Set the position and rotation for the projectile
*/
void ATC_Projectile::ProjectileBehaviorAfterHit()
{
	// Check if already bounce
	if (!AlreadyBounced)
	{
		// if it hits an enemy
		if (ProjectileHitEnemy)
		{
			ProjectileMovementComponent->Velocity += ProjectileVelocityAfterHit;
		}

		AlreadyBounced = true;
		// deactivate the damage for enemies
		ProjectileMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
		// enable overlap with player
		ProjectileMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	else
	{
		// if yes deactivate velocity, so that it only bounces once
		SetActorRotation(ProjectileRotationAfterHit);
		FVector ProjectilePosition = GetActorLocation();
		ProjectilePosition.Z = ProjectilePosition.Z + 10.f;
		SetActorLocation(ProjectilePosition);
		ProjectileMovementComponent->Velocity = FVector(0.f, 0.f, 0.f);
	}
}

/*
* Function that will be used if the player overlapps a projectile that moves - to recover it
*/
void ATC_Projectile::RecoverProjectile(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// if the overlapped actor is the player
	if (Other == DonutPlayer)
	{
		DestroyProjectile();
		DonutPlayer->ReloadProjectilesAfterOverlap(ProjectileType, ProjectileHitEnemy);
	}
}