// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_Boss.h"
#include "TC_DonutPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "AIController.h"
#include "../Environment/TC_Room.h"
#include "../TC_GameMode.h"
#include "../Actors/TC_Projectile.h"
#include "../Actors/Collectables/TC_Collectable.h"
#include "Components/SphereComponent.h"

ATC_Boss::ATC_Boss(const FObjectInitializer& OI) : Super(OI)
{
  BaseTurret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base Boss"));
  BaseTurret->SetupAttachment(GetMesh());
  TurningTurret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turning Turret Boss"));
  TurningTurret->SetupAttachment(BaseTurret);
  TurningSecondTurret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turning Second Turret Boss"));
  TurningSecondTurret->SetupAttachment(TurningTurret);
  TurningThirdTurret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turning Third Turret Boss"));
  TurningThirdTurret->SetupAttachment(TurningTurret);

  SecondProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Second Projectile Spawn Point"));
  SecondProjectileSpawnPoint->SetupAttachment(RootComponent);
  ThirdProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Third Projectile Spawn Point"));
  ThirdProjectileSpawnPoint->SetupAttachment(RootComponent);

  SphereToCheckPlayerDistance = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Player Detection"));
  SphereToCheckPlayerDistance->SetupAttachment(RootComponent);
  SphereToCheckPlayerDistance->SetSphereRadius(200.0f);
}

// Called every frame
void ATC_Boss::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  RotateMesh(Donut->GetActorLocation());
}

void ATC_Boss::HandleDestruction()
{

  if (ExplosionSound)
  {
    UGameplayStatics::PlaySound2D(this, ExplosionSound, 0.4f, 4.0f);
  }

  // ignore the collisions
  GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
  GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);

  // Hide actor in game deactivate
  ActivateEnemy(false);

  // Exlosion effect
  TurretExplosionEffect();

  //// Timer to destroy the enemy
  FTimerHandle EnemyDestroyTimer;
  GetWorldTimerManager().SetTimer(EnemyDestroyTimer, this, &ATC_Boss::DestroyEnemy, DestroyDelay, false);

  SpawnCollectable();
}

// Called when the game starts or when spawned
void ATC_Boss::BeginPlay()
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

  // when the player enters the sphere component
  SphereToCheckPlayerDistance->OnComponentBeginOverlap.AddDynamic(this, &ATC_Boss::OnPlayerBeginOverlap);

  // SetTimer
  GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATC_Boss::Fire, FireRate, true, FirstFireDelay);
}

void ATC_Boss::Fire()
{
  if (!bCanFire || !bCharacterActive || !FireProjectileClass)
    return;

  ATC_Projectile* Projectile = GetWorld()->SpawnActor<ATC_Projectile>(FireProjectileClass,
    ProjectileSpawnPoint->GetComponentLocation(),
    ProjectileSpawnPoint->GetComponentRotation());
  Projectile->SetActorScale3D(ProjectileScale);
  ATC_Projectile* Projectile2 = GetWorld()->SpawnActor<ATC_Projectile>(FireProjectileClass,
    SecondProjectileSpawnPoint->GetComponentLocation(),
    SecondProjectileSpawnPoint->GetComponentRotation());
  Projectile2->SetActorScale3D(ProjectileScale);
  ATC_Projectile* Projectile3 = GetWorld()->SpawnActor<ATC_Projectile>(FireProjectileClass,
    ThirdProjectileSpawnPoint->GetComponentLocation(),
    ThirdProjectileSpawnPoint->GetComponentRotation());
  Projectile3->SetActorScale3D(ProjectileScale);

  // So the projectile know who is the owner
  Projectile->SetOwner(this);
  Projectile2->SetOwner(this);
  Projectile3->SetOwner(this);
}

void ATC_Boss::SpawnCollectable()
{
  // if no collectables or it was already spawned
  if (Collectables.Num() == 0 || bCollectableAlreadySpawned)
    return;

  GetWorld()->SpawnActor<ATC_Collectable>(Collectables[0], GetActorLocation(), FRotator(0.f, 0.f, 0.f));
  bCollectableAlreadySpawned = true;
}