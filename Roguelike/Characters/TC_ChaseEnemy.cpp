// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_ChaseEnemy.h"
#include "TC_DonutPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "../TC_GameMode.h"
#include "../Environment/TC_Room.h"
#include "Components/SphereComponent.h"
#include "../ActorComponents/TC_HealthComponent.h"

ATC_ChaseEnemy::ATC_ChaseEnemy(const FObjectInitializer& OI) : Super(OI)
{
	SphereToCheckPlayerDistance = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Player Detection"));
	SphereToCheckPlayerDistance->SetupAttachment(RootComponent);
	SphereToCheckPlayerDistance->SetSphereRadius(200.0f);
}


void ATC_ChaseEnemy::SetMaterials()
{
	// if no predifined skin was selected, define it
	if (CharacterSkin == ESkin::None)
	{
		int RandomSkin = FMath::RandRange(0, 2);
		if (RandomSkin == 0)
		{
			// Fire Skin
			CharacterSkin = ESkin::Fire;
		}
		else if (RandomSkin == 1)
		{
			// Water Skin
			CharacterSkin = ESkin::Water;
		}
		else
		{
			// Plant Skin
			CharacterSkin = ESkin::Plant;
		}
	}

	if (CharacterSkin == ESkin::Fire)
	{
		GetMesh()->SetMaterial(0, FireSkin1);
		GetMesh()->SetMaterial(2, FireSkin3);
	}
	else if (CharacterSkin == ESkin::Water)
	{
		GetMesh()->SetMaterial(0, WaterSkin1);
		GetMesh()->SetMaterial(2, WaterSkin3);
	}
	else
	{
		GetMesh()->SetMaterial(0, PlantSkin1);
		GetMesh()->SetMaterial(2, PlantSkin3);
	}
}

// Called every frame
void ATC_ChaseEnemy::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

	ChasePlayer();
}

void ATC_ChaseEnemy::HandleDestruction()
{
	ChaseEnemyDestroying = true;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);

	if (!ExplosionSound)
		return;

	// Hide actor in game
	GetMesh()->SetVisibility(false);

	UGameplayStatics::PlaySound2D(this, ExplosionSound, 0.4f, 4.0f);
	// Exlosion effect
	EnemyExplosionEffect();

	
	// Timer to destroy the enemy
	FTimerHandle EnemyDestroyTimer;
	GetWorldTimerManager().SetTimer(EnemyDestroyTimer, this, &ATC_ChaseEnemy::DestroyEnemy, DestroyDelay, false);

	SpawnCollectable();
}

// Called when the game starts or when spawned
void ATC_ChaseEnemy::BeginPlay()
{
  Super::BeginPlay();

	SetMaterials();

	DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!DonutGameMode)
		return;

	// execute onhit function when the mesh touches something
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ATC_ChaseEnemy::OnHit);

  // Get a reference to the players pawn
  Donut = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));


  // get the controller of the enemy
	SpawnDefaultController();
  EnemyChaseController = Cast<AAIController>(GetController());

	// when the player enters the sphere component
	SphereToCheckPlayerDistance->OnComponentBeginOverlap.AddDynamic(this, &ATC_ChaseEnemy::OnPlayerBeginOverlap);
}

// Function to search for the player and chase him
void ATC_ChaseEnemy::ChasePlayer()
{
	GetMesh()->SetRelativeRotation(GetVelocity().Rotation() + FRotator(0.f, 90.f, 0.f));

	if (!EnemyChaseController)
		return;

	if (bCharacterActive)
	{
		EnemyChaseController->MoveToLocation(Donut->GetActorLocation());
	}
	else
	{
		EnemyChaseController->StopMovement();
	}
}

/*
* Function used for the mesh hit
*/
void ATC_ChaseEnemy::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor)
		return;

	// if it has no owner
	if (!GetOwner())
		return;

	// if the otheractor exist, is not this actor
	if (OtherActor && OtherActor != this)
	{
		// Do not apply damage to other enemies
		if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_GameTraceChannel2)
		{
			return;
		}

		
		if (OtherActor == Donut && !ChaseEnemyDestroying)
		{
			UE_LOG(LogTemp, Warning, TEXT("Te has muerto o no, trozo de mierda"));
			ChaseEnemyDestroying = true;
			
			// apply the damage to the other actor
			UGameplayStatics::ApplyDamage(OtherActor, Damage, EnemyChaseController, this, UDamageType::StaticClass());

			DonutGameMode->ActorDied(this);
		}
	}
}