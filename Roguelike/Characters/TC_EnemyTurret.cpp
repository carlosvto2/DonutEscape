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
#include "Components/PrimitiveComponent.h"

ATC_EnemyTurret::ATC_EnemyTurret(const FObjectInitializer& OI) : Super(OI)
{
  BaseTurret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base turret"));
  BaseTurret->SetupAttachment(GetMesh());
  TurningTurret = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Turning turret"));
  TurningTurret->SetupAttachment(BaseTurret);

  SphereToCheckPlayerDistance = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Player Detection"));
  SphereToCheckPlayerDistance->SetupAttachment(RootComponent);
  SphereToCheckPlayerDistance->SetSphereRadius(200.0f);

}

void ATC_EnemyTurret::SetMaterials()
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
    CharacterSkin = ESkin::Fire;
    TurningTurret->SetMaterial(0, FireSkin1);
    TurningTurret->SetMaterial(1, FireSkin2);
    TurningTurret->SetMaterial(2, FireSkin3);
    TurningTurret->SetMaterial(3, FireSkin4);
    TurningTurret->SetMaterial(4, FireSkin5);
    ProjectileUsing = FireProjectileClass;
  }
  else if (CharacterSkin == ESkin::Water)
  {
    // Water Skin
    CharacterSkin = ESkin::Water;
    TurningTurret->SetMaterial(0, WaterSkin1);
    TurningTurret->SetMaterial(1, WaterSkin2);
    TurningTurret->SetMaterial(2, WaterSkin3);
    TurningTurret->SetMaterial(3, WaterSkin4);
    TurningTurret->SetMaterial(4, WaterSkin5);
    ProjectileUsing = WaterProjectileClass;
  }
  else
  {
    // Plant Skin
    CharacterSkin = ESkin::Plant;
    TurningTurret->SetMaterial(0, PlantSkin1);
    TurningTurret->SetMaterial(1, PlantSkin2);
    TurningTurret->SetMaterial(2, PlantSkin3);
    TurningTurret->SetMaterial(3, PlantSkin4);
    TurningTurret->SetMaterial(4, PlantSkin5);
    ProjectileUsing = PlantProjectileClass;
  }
}

// Called every frame
void ATC_EnemyTurret::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  RotateMesh(Donut->GetActorLocation());
}

void ATC_EnemyTurret::HandleDestruction()
{
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

  //// Timer to destroy the enemy
  FTimerHandle EnemyDestroyTimer;
  GetWorldTimerManager().SetTimer(EnemyDestroyTimer, this, &ATC_EnemyTurret::DestroyEnemy, DestroyDelay, false);

  SpawnCollectable();
}

// Called when the game starts or when spawned
void ATC_EnemyTurret::BeginPlay()
{
  Super::BeginPlay();
  SetMaterials();

  //ProjectileUsing = FireProjectileClass;

  DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));
  if (!DonutGameMode)
    return;

  // execute onhit function when the mesh touches something
  GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ATC_EnemyTurret::OnHit);

  // Get a reference to the players pawn
  Donut = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));

  // get the controller of the enemy
  SpawnDefaultController();
  EnemyTurretController = Cast<AAIController>(GetController());

  // when the player enters the sphere component
  SphereToCheckPlayerDistance->OnComponentBeginOverlap.AddDynamic(this, &ATC_EnemyTurret::OnPlayerBeginOverlap);

  // SetTimer
  GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &ATC_EnemyTurret::Fire, FireRate, true, FirstFireDelay);
}

void ATC_EnemyTurret::Fire()
{
  if (!bCanFire || !bCharacterActive || !ProjectileUsing)
    return;

  ATC_Projectile* Projectile = GetWorld()->SpawnActor<ATC_Projectile>(ProjectileUsing,
    ProjectileSpawnPoint->GetComponentLocation(),
    ProjectileSpawnPoint->GetComponentRotation());
  // So the projectile know who is the owner
  Projectile->SetOwner(this);
}

void ATC_EnemyTurret::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
  if (!OtherComp || OtherComp->IsPendingKill())
    return;

  // it is a component the player can grab and throw
  if (OtherComp->ComponentHasTag("Grab"))
  {
    float Damage = OtherComp->GetComponentVelocity().Size() / 5;
    if (Damage > 50)
    {
      if (!GetHealthComponent())
      {
        UE_LOG(LogTemp, Error, TEXT("HEALTH COMPONENT NOT CREATED"));
        return;
      }
      // apply the damage to the other actor
      UGameplayStatics::ApplyDamage(this, Damage, EnemyTurretController, this, UDamageType::StaticClass());
    }
  }
}

/* COUNTER ATTACKS */

/*
* WATER Counter attack of the turret for a FIRE projectile
*/
void ATC_EnemyTurret::CounterWaterAttackToFireProjectile()
{
  
}
/*
* FIRE Counter attack of the turret for a PLANT projectile
*/
void ATC_EnemyTurret::CounterFireAttackToPlantProjectile()
{

}
/*
* PLANT Counter attack of the turret for a WATER projectile
*/
void ATC_EnemyTurret::CounterPlantAttackToWaterProjectile()
{

}