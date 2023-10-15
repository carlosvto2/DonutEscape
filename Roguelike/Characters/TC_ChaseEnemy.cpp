// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_ChaseEnemy.h"
#include "TC_DonutPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "../TC_GameMode.h"
#include "../Environment/TC_Room.h"

// Called every frame
void ATC_ChaseEnemy::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

	ChasePlayer();
}

void ATC_ChaseEnemy::HandleDestruction()
{
	if (!RoomOwner)
		return;

	if (!ExplosionSound)
		return;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);

	// Hide actor in game
	GetMesh()->SetVisibility(false);
	UGameplayStatics::PlaySound2D(this, ExplosionSound, 0.4f, 4.0f);
	// Exlosion effect
	EnemyExplosionEffect();

	RoomOwner->CheckIfLastEnemy(this);
	
	// Timer to destroy the enemy
	FTimerHandle EnemyDestroyTimer;
	GetWorldTimerManager().SetTimer(EnemyDestroyTimer, this, &ATC_ChaseEnemy::DestroyEnemy, DestroyDelay, false);

	SpawnCollectable();
}

// Called when the game starts or when spawned
void ATC_ChaseEnemy::BeginPlay()
{
  Super::BeginPlay();

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
		// apply the damage to the other actor
		UGameplayStatics::ApplyDamage(OtherActor, Damage, EnemyChaseController, this, UDamageType::StaticClass());

		if (OtherActor == Donut)
		{
			DonutGameMode->ActorDied(this);
		}
	}
}