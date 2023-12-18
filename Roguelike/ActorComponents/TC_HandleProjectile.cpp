// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_HandleProjectile.h"
#include "../Actors/TC_Projectile.h"
#include "../Characters/TC_DonutPlayer.h"
#include "../TC_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "../Animations/TC_AnimInstance.h"

// Sets default values for this component's properties
UTC_HandleProjectile::UTC_HandleProjectile()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UTC_HandleProjectile::BeginPlay()
{
	Super::BeginPlay();

  DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));
  if (!DonutGameMode)
    return;

  DonutPlayer = Cast<ATC_DonutPlayer>(GetOwner());
  if (!DonutPlayer)
    return;

  TypeUsingProjectile = EProjectile::Water;

  SaveProjectilesInformation();

}

void UTC_HandleProjectile::SaveProjectilesInformation()
{
  // Information for fire projectile
  ATC_Projectile* FireProjectile = DonutPlayer->FireProjectileClass->GetDefaultObject<ATC_Projectile>();
  FireProjInformation.SetProjectileClass(DonutPlayer->FireProjectileClass);
  FireProjInformation.SetSideEffectAfterRecover(FireProjectile->GetEffectToApplyAfterRecover());
  FireProjInformation.SetProjectileType(EProjectile::Fire);
  FireProjInformation.SetCadencyProjectile(FireProjectile->GetProjectileCadency());
  FireProjInformation.SetMaxProjectilesToUse(FireProjectile->GetMaxProjectilesToUse());
  FireProjInformation.SetProjectilesLeft(FireProjectile->GetMaxProjectilesToUse());
  FireProjInformation.SetIMovingProjectile(FireProjectile->MovingProjectile);
  FireProjInformation.SetIfThrowingFromHand(FireProjectile->ThrowingFromHand);

  // Information for water projectile
  ATC_Projectile* WaterProjectile = DonutPlayer->WaterProjectileClass->GetDefaultObject<ATC_Projectile>();
  WaterProjInformation.SetProjectileClass(DonutPlayer->WaterProjectileClass);
  WaterProjInformation.SetSideEffectAfterRecover(WaterProjectile->GetEffectToApplyAfterRecover());
  WaterProjInformation.SetProjectileType(EProjectile::Water);
  WaterProjInformation.SetCadencyProjectile(WaterProjectile->GetProjectileCadency());
  WaterProjInformation.SetMaxProjectilesToUse(WaterProjectile->GetMaxProjectilesToUse());
  WaterProjInformation.SetProjectilesLeft(WaterProjectile->GetMaxProjectilesToUse());
  WaterProjInformation.SetIMovingProjectile(WaterProjectile->MovingProjectile);
  WaterProjInformation.SetIfThrowingFromHand(WaterProjectile->ThrowingFromHand);

  // Information for plant projectile
  ATC_Projectile* PlantProjectile = DonutPlayer->PlantProjectileClass->GetDefaultObject<ATC_Projectile>();
  PlantProjInformation.SetProjectileClass(DonutPlayer->PlantProjectileClass);
  PlantProjInformation.SetSideEffectAfterRecover(PlantProjectile->GetEffectToApplyAfterRecover());
  PlantProjInformation.SetProjectileType(EProjectile::Plant);
  PlantProjInformation.SetCadencyProjectile(PlantProjectile->GetProjectileCadency());
  PlantProjInformation.SetMaxProjectilesToUse(PlantProjectile->GetMaxProjectilesToUse());
  PlantProjInformation.SetProjectilesLeft(PlantProjectile->GetMaxProjectilesToUse());
  PlantProjInformation.SetIMovingProjectile(PlantProjectile->MovingProjectile);
  PlantProjInformation.SetIfThrowingFromHand(PlantProjectile->ThrowingFromHand);

  if (!DonutGameMode)
    return;
  // set all the information in the widget
  DonutGameMode->UpdateNumberOfProjectiles(FireProjInformation.GetMaxProjectilesToUse(),
    WaterProjInformation.GetMaxProjectilesToUse(), PlantProjInformation.GetMaxProjectilesToUse());
}


// Called every frame
void UTC_HandleProjectile::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UTC_HandleProjectile::GetProjectileInformationFromType(EProjectile ProjectileType,
   FUProjectileInformation& ProjInformation)
{
  // set the next projectile using
  switch (ProjectileType)
  {
  case EProjectile::Fire:
    ProjInformation = FireProjInformation;
    break;

  case EProjectile::Water:
    ProjInformation = WaterProjInformation;
    break;

  case EProjectile::Plant:
    ProjInformation = PlantProjInformation;
    break;
  }
}

void UTC_HandleProjectile::SetProjectileOfFire()
{
  SetProjectileInformation(EProjectile::Fire);
  if (!DonutGameMode)
  {
    UE_LOG(LogTemp, Error, TEXT("DonutGameMode NOT CREATED"));
    return;
  }
  DonutGameMode->UpdateProjectileTypeWidget(EProjectile::Fire);
}
void UTC_HandleProjectile::SetProjectileOfWater()
{
  SetProjectileInformation(EProjectile::Water);
  if (!DonutGameMode)
  {
    UE_LOG(LogTemp, Error, TEXT("DonutGameMode NOT CREATED"));
    return;
  }
  DonutGameMode->UpdateProjectileTypeWidget(EProjectile::Water);
}
void UTC_HandleProjectile::SetProjectileOfPlant()
{
  SetProjectileInformation(EProjectile::Plant);
  if (!DonutGameMode)
  {
    UE_LOG(LogTemp, Error, TEXT("DonutGameMode NOT CREATED"));
    return;
  }
  DonutGameMode->UpdateProjectileTypeWidget(EProjectile::Plant);
}


void UTC_HandleProjectile::SetProjectileInformation(EProjectile ProjectileType)
{
  // if already using this projectile, dont try to change
  if (ProjectileType == TypeUsingProjectile)
    return;

  ProjectileSpawnPoint = DonutPlayer->ProjectileSpawnPoint;

  // ProjectileUsing was already initialized before
  if (ProjectileUsing.GetMaxProjectilesToUse() != 0)
  {
    // Check the previous type and save the information in the correspondant struct information
    switch (TypeUsingProjectile)
    {
    case EProjectile::Fire:
      FireProjInformation = ProjectileUsing;
      break;

    case EProjectile::Water:
      WaterProjInformation = ProjectileUsing;
      break;

    case EProjectile::Plant:
      PlantProjInformation = ProjectileUsing;
      break;
    }
  }
  
  TypeUsingProjectile = ProjectileType;


  // get the projectile information and save in ProjectileUsing
  GetProjectileInformationFromType(ProjectileType, ProjectileUsing);
}

bool UTC_HandleProjectile::MouseInRange()
{
  float DistanceToPoint = FVector::Dist(GetOwner()->GetActorLocation(), MousePosition.ImpactPoint);
  if (DistanceToPoint < MinDistanceForStaticProjectiles)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void UTC_HandleProjectile::CheckProjectileRange(FHitResult HitCameraMouse)
{
  if (!DonutGameMode)
    return;

  // save the position of the mouse in the map
  MousePosition = HitCameraMouse;

  if (ProjectileUsing.GetIfMovingProjectile())
  {
    DonutGameMode->UpdateProjectileRangeColor(false, 0.0f);
  }
  else
  {
    // tell the widget if the mouse is inside of the allowed range
    DonutGameMode->UpdateProjectileRangeColor(MouseInRange(), 1.0f);
  }
}

void UTC_HandleProjectile::ProjectileOfFire()
{
  /*if (!bMultipleShoot)
  {*/
  if (!ProjectileUsing.GetProjectileClass())
    return;

    ATC_Projectile* Projectile = GetWorld()->SpawnActor<ATC_Projectile>(ProjectileUsing.GetProjectileClass(),
      ProjectileSpawnPoint->GetComponentLocation(),
      ProjectileSpawnPoint->GetComponentRotation());

    // So the projectile know who is the owner
    Projectile->SetOwner(GetOwner());
  //}
  //else
  //{
  //  // if the player has the buff to shoot two projectiles

  //  ATC_Projectile* Projectile = GetWorld()->SpawnActor<ATC_Projectile>(ProjectileClass,
  //    ProjectileSpawnPoint->GetComponentLocation(),
  //    ProjectileSpawnPoint->GetComponentRotation());
  //  Projectile->SetProjectileType(EProjectile::Fire);
  //  Projectile->AddActorLocalOffset(FVector(0.f, 25.f, 0.f));
  //  // So the projectile know who is the owner
  //  Projectile->SetOwner(this);

  //  ATC_Projectile* Projectile2 = GetWorld()->SpawnActor<ATC_Projectile>(ProjectileClass,
  //    ProjectileSpawnPoint->GetComponentLocation(),
  //    ProjectileSpawnPoint->GetComponentRotation());
  //  Projectile2->SetProjectileType(EProjectile::Fire);
  //  Projectile2->AddActorLocalOffset(FVector(0.f, -25.f, 0.f));
  //  // So the projectile know who is the owner
  //  Projectile2->SetOwner(this);
  //}
}

void UTC_HandleProjectile::FireProjectile()
{
  // still not reloaded
  if (ProjectileUsing.GetProjectilesLeft() <= 0)
    return;


  // Check if the projectile will move, or spawns in a particular position
  if (!ProjectileUsing.GetIfMovingProjectile() && !ProjectileUsing.GetIfThrowingFromHand())
  {
    // set location and rotation spawn for static projectile
    // Check the distance to the clicked point
    if (!MouseInRange())
      return;

    ProjectileSpawnPosition = MousePosition.ImpactPoint;
    ProjectileSpawnRotation = FRotator::ZeroRotator;
  }
  

  if (!ProjectileUsing.GetProjectileClass())
    return;


  // decrement number of projectiles available
  int ProjLeft = ProjectileUsing.GetProjectilesLeft() - 1;
  ProjectileUsing.SetProjectilesLeft(ProjLeft);

  if (!ProjectileUsing.GetIfThrowingFromHand())
  {
    SpawnProjectile();
  }
  else
  {
    // Create a timer to spawn the projectile after some time
    SpawnProjectile();
    //DonutPlayer->CreateGenericTimer(this, &UTC_HandleProjectile::SpawnProjectile, 0.4f);
  }

  // if no more projectiles, reload
  if (ProjectileUsing.GetProjectilesLeft() <= 0)
  {
    // Create a timer to reload projectiles
    DonutPlayer->TimerToReloadProjectiles(ProjectileUsing.GetCadencyProjectile(), 
      ProjectileUsing.GetProjectileType());
  }
}

void UTC_HandleProjectile::SpawnProjectile()
{
  // if the projectile moves
  if (ProjectileUsing.GetIfMovingProjectile())
  {
    if (ProjectileUsing.GetIfThrowingFromHand())
    {
      DonutPlayer->ExecuteAnimMontage("Throw");
      // if the projectile should spawn in the hand
      ProjectileSpawnPosition = DonutPlayer->GetSphereGrabber()->GetComponentLocation();
      // get the rotation to throw the projectile, depending on the mouse position
      FVector VectorTarget = MousePosition.ImpactPoint - DonutPlayer->GetSphereGrabber()->GetComponentLocation();
      ProjectileSpawnRotation = VectorTarget.Rotation() + DonutPlayer->AimOffset;
    }
    else
    {
      // set location and rotation spawn for moving projectile
      ProjectileSpawnPosition = ProjectileSpawnPoint->GetComponentLocation();
      ProjectileSpawnRotation = ProjectileSpawnPoint->GetComponentRotation();
    }
  }
  
  if (!ProjectileUsing.GetProjectileClass())
    return;

  ATC_Projectile* Projectile = GetWorld()->SpawnActor<ATC_Projectile>(ProjectileUsing.GetProjectileClass(),
    ProjectileSpawnPosition + FVector(0.f, 0.f, 2.f),
    ProjectileSpawnRotation);

  if (!Projectile)
    return;
  // So the projectile know who is the owner
  Projectile->SetOwner(DonutPlayer);

  // Set the skin of the player when shooting
  Projectile->SetPlayerSkinWhenFiringProjectile(DonutPlayer->GetCharacterSkin());

  
  // Update projectiles left in widget
  DonutGameMode->UpdateNumberOfProjectilesLeftWidget(ProjectileUsing.GetProjectileType(),
    ProjectileUsing.GetProjectilesLeft(), ProjectileUsing.GetMaxProjectilesToUse());
}

void UTC_HandleProjectile::ReloadProjectiles(EProjectile ProjectileTypeToReload)
{
  int ProjectilesLeft = 0;
  int MaxProjectiles = 0;

  // set the projectiles left for the correspondant projectile type
  switch (ProjectileTypeToReload)
  {
    case EProjectile::Fire:

      // Set the projectiles left to the maximum
      FireProjInformation.SetProjectilesLeft(FireProjInformation.GetMaxProjectilesToUse());
      ProjectilesLeft = FireProjInformation.GetProjectilesLeft();
      MaxProjectiles = FireProjInformation.GetMaxProjectilesToUse();

      // if currently using this projectile type, also saved the reloaded projectiles
      if (ProjectileUsing.GetProjectileType() == EProjectile::Fire)
        ProjectileUsing = FireProjInformation;
      break;

    case EProjectile::Water:
      WaterProjInformation.SetProjectilesLeft(WaterProjInformation.GetMaxProjectilesToUse());
      ProjectilesLeft = WaterProjInformation.GetProjectilesLeft();
      MaxProjectiles = WaterProjInformation.GetMaxProjectilesToUse();

      // if currently using this projectile type, also saved the reloaded projectiles
      if (ProjectileUsing.GetProjectileType() == EProjectile::Water)
        ProjectileUsing = WaterProjInformation;
      break;

    case EProjectile::Plant:
      PlantProjInformation.SetProjectilesLeft(PlantProjInformation.GetMaxProjectilesToUse());
      ProjectilesLeft = PlantProjInformation.GetProjectilesLeft();
      MaxProjectiles = PlantProjInformation.GetMaxProjectilesToUse();

      // if currently using this projectile type, also saved the reloaded projectiles
      if (ProjectileUsing.GetProjectileType() == EProjectile::Plant)
        ProjectileUsing = PlantProjInformation;
      break;
  }

  // Update projectiles left in widget
  DonutGameMode->UpdateNumberOfProjectilesLeftWidget(ProjectileTypeToReload,
    ProjectilesLeft, MaxProjectiles);
}

/*
* Update the projectile information about the number of the stacks of the effect and damage
*/
void UTC_HandleProjectile::UpdateProjectileStackInformation(FUProjectileInformation ProjectileToUpdate,
  int Stacks, float IncrementDamagePercentage)
{
  switch (ProjectileToUpdate.GetProjectileType())
  {
    case EProjectile::Fire:
      FireProjInformation.SetStacksAccumulated(Stacks);
      FireProjInformation.SetEffectIncrementDamage(IncrementDamagePercentage);

      // if currently using this projectile type, also saved the reloaded projectiles
      if (ProjectileUsing.GetProjectileType() == EProjectile::Fire)
        ProjectileUsing = FireProjInformation;
      break;

    case EProjectile::Water:
      WaterProjInformation.SetStacksAccumulated(Stacks);
      WaterProjInformation.SetEffectIncrementDamage(IncrementDamagePercentage);

      // if currently using this projectile type, also saved the reloaded projectiles
      if (ProjectileUsing.GetProjectileType() == EProjectile::Water)
        ProjectileUsing = WaterProjInformation;
      break;

    case EProjectile::Plant:
      PlantProjInformation.SetStacksAccumulated(Stacks);
      PlantProjInformation.SetEffectIncrementDamage(IncrementDamagePercentage);

      // if currently using this projectile type, also saved the reloaded projectiles
      if (ProjectileUsing.GetProjectileType() == EProjectile::Plant)
        ProjectileUsing = PlantProjInformation;
      break;
  }
}