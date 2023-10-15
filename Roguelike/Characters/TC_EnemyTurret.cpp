// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_EnemyTurret.h"
#include "TC_DonutPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "AIController.h"
#include "../Environment/TC_Room.h"
#include "../TC_GameMode.h"

ATC_EnemyTurret::ATC_EnemyTurret()
{
  BaseTurret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base turret"));
  BaseTurret->SetupAttachment(GetMesh());
  TurningTurret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turning turret"));
  TurningTurret->SetupAttachment(BaseTurret);
}

// Called every frame
void ATC_EnemyTurret::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  RotateMesh(Donut->GetActorLocation());
}

void ATC_EnemyTurret::HandleDestruction()
{
  if (!RoomOwner)
    return;

  if (!ExplosionSound)
    return;

  // ignore the collisions
  GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
  GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);

  // Hide actor in game deactivate
  ActivateEnemy(false);

  UGameplayStatics::PlaySound2D(this, ExplosionSound, 0.4f, 4.0f);
  // Exlosion effect
  TurretExplosionEffect();

  RoomOwner->CheckIfLastEnemy(this);

  //// Timer to destroy the enemy
  FTimerHandle EnemyDestroyTimer;
  GetWorldTimerManager().SetTimer(EnemyDestroyTimer, this, &ATC_EnemyTurret::DestroyEnemy, DestroyDelay, false);

  SpawnCollectable();
}

// Called when the game starts or when spawned
void ATC_EnemyTurret::BeginPlay()
{
  Super::BeginPlay();

  DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));
  if (!DonutGameMode)
    return;

  // Get a reference to the players pawn
  Donut = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));

  // get the controller of the enemy
  SpawnDefaultController();
  EnemyTurretController = Cast<AAIController>(GetController());

  // SetTimer
  GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATC_EnemyTurret::Fire, FireRate, true, FirstFireDelay);
}