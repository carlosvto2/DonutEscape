// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_DonutPlayer.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "../Actors/Collectables/TC_Collectable.h"
#include "../Actors/Collectables/TC_CollectableKey.h"
#include "../Actors/Collectables/TC_MagicBox.h"
#include "../Actors/Collectables/TC_BombCollectable.h"
#include "../Actors/Collectables/TC_MultipleShoot.h"
#include "../Actors/Collectables/TC_HealthIncrement.h"
#include "../Actors/Collectables/TC_Shield.h"
#include "../Actors/Collectables/TC_PowerUnit.h"
#include "../Actors/Collectables/TC_Coin.h"
#include "../Actors/Collectables/TC_MagicGem.h"
#include "../Actors/TC_Door.h"
#include "../ActorComponents/TC_MoveComponent.h"
#include "../ActorComponents/TC_HealthComponent.h"
#include "../ActorComponents/TC_HandleProjectile.h"
#include "../ActorComponents/TC_StatusComponent.h"
#include "../Controllers/TC_DonutPlayerController.h"
#include "../Actors/TC_Projectile.h"
#include "../TC_GameMode.h"
#include "Engine/EngineTypes.h"
#include "../Animations/TC_AnimInstance.h"

ATC_DonutPlayer::ATC_DonutPlayer(const FObjectInitializer& OI) : Super(OI)
{
  SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
  SpringArm->SetupAttachment(RootComponent);

  // Do not inherit rotation from parent component
  SpringArm->bInheritPitch = false;
  SpringArm->bInheritRoll = false;
  SpringArm->bInheritYaw = false;

  Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
  Camera->SetupAttachment(SpringArm);

  // Define the sphere where the player will grab the collectors
  SphereCollector = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere collector"));
  SphereCollector->SetupAttachment(RootComponent);
  SphereCollector->SetSphereRadius(200.0f);

  SphereGrabber = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere grabber"));
  SphereGrabber->SetupAttachment(GetMesh(), TEXT("GrabSocket"));

  // Add shield
  //PlayerShield = CreateDefaultSubobject<UTC_HealthComponent>(TEXT("Shield"));

  // Add handle projectile component
  HandleProjectileComponent = OI.CreateDefaultSubobject<UTC_HandleProjectile>(this, TEXT("HandleProjectileComponent"));
}

#pragma region CONTROLBEGINTICK

void ATC_DonutPlayer::SetPrincipalVariableValues()
{
  DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));
  if (!DonutGameMode)
    return;


  // Projectile
  if (!HandleProjectileComponent)
  {
    UE_LOG(LogTemp, Error, TEXT("HandleProjectileComponent NOT CREATED"));
    return;
  }


  // Set the projectile of fire as first projectile to use
  HandleProjectileComponent->SetProjectileOfFire();

  // SpringArm Rotation
  SpringArmActualRotation = SpringArm->GetComponentRotation();

  // Character variables
  KeyPicked = false;
  bCharacterActive = true;
  ItemGrabbed = false;

  // animation instance
  AnimInst = Cast<UTC_AnimInstance>(GetMesh()->GetAnimInstance());

  // player controller
  APlayerController* PlayerController = Cast<APlayerController>(GetController());
  DonutPlayerController = Cast<ATC_DonutPlayerController>(PlayerController);
  // shield component (Health component)
  //PlayerShield->SetComponentAsShield(true);
}

// Called when the game starts or when spawned
void ATC_DonutPlayer::BeginPlay()
{
  Super::BeginPlay();

  SetPrincipalVariableValues();
  
  //Add Input Mapping Context
  if (DonutPlayerController)
  {
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(DonutPlayerController->GetLocalPlayer()))
    {
      Subsystem->AddMappingContext(MappingContext, 0);
      TArray<FEnhancedActionKeyMapping> a = Subsystem->GetAllPlayerMappableActionKeyMappings();
      for (FEnhancedActionKeyMapping b : a)
      {
        //UE_LOG(LogTemp, Warning, TEXT("key: %s"), b.Key.GetFName().ToString());
      }
    }
  }
}

void ATC_DonutPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  // Set up action bindings
  if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
    //Moving - input action bindind to move function
    EnhancedInputComponent->BindAction(IMove, ETriggerEvent::Triggered, this, &ATC_DonutPlayer::Move);

    // Fire
    EnhancedInputComponent->BindAction(IFire, ETriggerEvent::Started, this, &ATC_DonutPlayer::Fire);
    // Bomb
    EnhancedInputComponent->BindAction(IBomb, ETriggerEvent::Started, this, &ATC_DonutPlayer::ThrowBomb);

    // Pick collectable
    EnhancedInputComponent->BindAction(IPick, ETriggerEvent::Started, this, &ATC_DonutPlayer::PickupCollector);

    // Defend
    EnhancedInputComponent->BindAction(IDefend, ETriggerEvent::Triggered, this, &ATC_DonutPlayer::DefendWithItem);
    EnhancedInputComponent->BindAction(IDefend, ETriggerEvent::Completed, this, &ATC_DonutPlayer::StopDefense);

    // Projectile change
    EnhancedInputComponent->BindAction(IFireProjectile, ETriggerEvent::Started, HandleProjectileComponent, &UTC_HandleProjectile::SetProjectileOfFire);
    EnhancedInputComponent->BindAction(IWaterProjectile, ETriggerEvent::Started, HandleProjectileComponent, &UTC_HandleProjectile::SetProjectileOfWater);
    EnhancedInputComponent->BindAction(IPlantProjectile, ETriggerEvent::Started, HandleProjectileComponent, &UTC_HandleProjectile::SetProjectileOfPlant);
  }
}

void ATC_DonutPlayer::Move(const FInputActionValue& Value)
{
  // input is a Vector2D that changes the Y axis
  FVector2D MovementVector = Value.Get<FVector2D>();

  FVector Direction = FVector(MovementVector.X, MovementVector.Y, 0.f);
  // direction of the movement + the final rotation the room had
  FRotator FinalDirection = Direction.Rotation() + FinalCameraRotationAngle;


  // add movement in the forward direction 
  AddMovementInput(FinalDirection.Vector(), 1);
}

void ATC_DonutPlayer::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (!DonutPlayerController)
  {
    UE_LOG(LogTemp, Error, TEXT("Donut player controller NOT CREATED"));
    return;
  }

  // Get the point where the cursor is hitting
  DonutPlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitCameraMouse);
  if (!HandleProjectileComponent)
  {
    UE_LOG(LogTemp, Error, TEXT("HandleProjectileComponent NOT CREATED"));
    return;
  }
  HandleProjectileComponent->CheckProjectileRange(HitCameraMouse);

  // rotate the character to the cursor location
  RotateMesh(HitCameraMouse.ImpactPoint);


  // Handle if there is any collectable around
  if (CheckIfOverlappingComponents())
  {
    DonutGameMode->ShowCollectableButtonWidget(true);
  }
  else
  {
    DonutGameMode->ShowCollectableButtonWidget(false);
  }

  if (bRotateCamera)
    RotateCamera();
}



void ATC_DonutPlayer::RotateCamera()
{
  bCameraRotating = true;

  FRotator TargetRotation = SpringArmActualRotation + CameraRotationAngle;

  // Interpolate between current rotation and target rotation, to go smoother
  SpringArm->SetRelativeRotation(FMath::RInterpTo(
    SpringArm->GetRelativeRotation(),
    TargetRotation,
    UGameplayStatics::GetWorldDeltaSeconds(this),
    CameraSpeedRotation)
  );

  // comparation Between SpringArm rotation and target Rotation
  
  bool CompareRotators = SpringArm->GetRelativeRotation().Equals(TargetRotation, 0.5f);

  // if the spring arm is already almost in the target rotation
  if (CompareRotators)
  {
    SpringArm->SetRelativeRotation(TargetRotation);
    SpringArmActualRotation = TargetRotation;
    bRotateCamera = false;
    bCameraRotating = false;
  }
}


void ATC_DonutPlayer::RotateCharacter(float RotationDegrees)
{
  bRotateCamera = true;
  // Partial rotation of camera this time
  CameraRotationAngle = FRotator(0.f, RotationDegrees, 0.f);
  // FINAL rotation of camera
  FinalCameraRotationAngle += CameraRotationAngle;

  while (FinalCameraRotationAngle.Yaw >= 360.f)
  {
    FinalCameraRotationAngle.Yaw -= 360.f;
  }
}

void ATC_DonutPlayer::HandleDestruction()
{
  Super::HandleDestruction();
  SetActorHiddenInGame(true);
  SetActorTickEnabled(false);
}

#pragma endregion CONTROLBEGINTICK

#pragma region COLLECTABLES

/*
* Function to hide the widget for all the collectables in the previous array
*/
void ATC_DonutPlayer::CollectablesHideWidgets()
{
  if (OverlappedComponents.Num() == 0)
    return;

  for (UPrimitiveComponent* OverlappedComponent : OverlappedComponents)
  {
    AActor* ComponentOwner = Cast<AActor>(OverlappedComponent->GetOwner());
    // if the component has owner/blueprint
    if (ComponentOwner)
    {
      ATC_Collectable* Collectable = Cast<ATC_Collectable>(ComponentOwner);
      if (!Collectable)
        return;
      Collectable->CollectableWidgetVisibility(false);
    }
  }
}

/*
* Get the overlapped actors and return true if there is any collectable
*/
bool ATC_DonutPlayer::CheckIfOverlappingComponents()
{
  // Hide the widget for all the previous overlapped components
  CollectablesHideWidgets();

  // Get all overlapping components and save them in an array
  SphereCollector->GetOverlappingComponents(OverlappedComponents);

  if (OverlappedComponents.Num() == 0)
    return false;

  for (UPrimitiveComponent* OverlappedComponent : OverlappedComponents)
  {
    // Get the owner/blueprint of the component
    AActor* ComponentOwner = Cast<AActor>(OverlappedComponent->GetOwner());
    // if the component has owner/blueprint
    if (ComponentOwner)
    {
      ATC_Collectable* Collectable = Cast<ATC_Collectable>(ComponentOwner);
      //if the overpped component is a collectable or it has the actor tag Grab
      if ((Collectable && Collectable->IsActive()) || OverlappedComponent->ComponentHasTag("Grab"))
      {
        // Show the info of the first collectable found
        Collectable->CollectableWidgetVisibility(true);
        return true;
      }
    }
  }
  return false;
}

/*
* When press E, the character will pick all the collectors within the spherecollector component
*/
void ATC_DonutPlayer::PickupCollector()
{
  CheckIfOverlappingComponents();

  for (UPrimitiveComponent* OverlappedComponent : OverlappedComponents)
  {

    // Get the owner/blueprint of the component
    AActor* ComponentOwner = Cast<AActor>(OverlappedComponent->GetOwner());
    // if exist can be a collectable, if not an object to grab

    if (ComponentOwner)
    {
      // IF COLLECTABLE
      ATC_Collectable* Collectable = Cast<ATC_Collectable>(ComponentOwner);
      //if the overpped actor is a collectable
      if (Collectable && Collectable->IsActive())
      {
        // check collectable to pick
        CastToPossibleCollectables(Collectable);
        return;
      }

      // IF DOOR
      ATC_Door* Door = Cast<ATC_Door>(ComponentOwner);
      if (Door)
      {
        // If the player has no key, return
        if (!KeyPicked)
        {
          DonutGameMode->ShowHelpTextWidget("Need a key to open the door");
          return;
        }


        UTC_MoveComponent* DoorMoveComponent = Door->GetMoveComponent();
        // if the door should not move yet, then move it
        if (DoorMoveComponent && !DoorMoveComponent->GetShouldMove())
        {
          DoorMoveComponent->SetShouldMove(true);

          HandleDestruction();
          // use controller to disable player
          DonutPlayerController->SetPlayerEnabledState(false, false);
          DonutGameMode->GameOver(true);
        }
        return;
      }
    }


    // OBJECT TO GRAB
    GrabItem(OverlappedComponent);

  }
}

/*
* Check what is the collectable to pick
*/
void ATC_DonutPlayer::CastToPossibleCollectables(ATC_Collectable* Collectable)
{
  // Check if the collectable is a KEY
  ATC_CollectableKey* Key = Cast<ATC_CollectableKey>(Collectable);
  if (Key)
  {
    // if it is a key, the player has picked the key
    KeyPicked = true;
    Key->PickUp();
  }
  else if (ATC_MagicBox* MagicBox = Cast<ATC_MagicBox>(Collectable))
  {
    // dont rotate if already rotating
    if (bCameraRotating)
      return;

    // If magicbox pick it up
    MagicBox->PickUp();
  }
  else if (ATC_BombCollectable* Bomb = Cast<ATC_BombCollectable>(Collectable)) {
    // pick up the collectable
    Bomb->PickUp();
  }
  else if (ATC_MultipleShoot* MultipleShoot = Cast<ATC_MultipleShoot>(Collectable)) {
    // pick up the collectable
    MultipleShoot->PickUp();
  }
  else if (ATC_HealthIncrement* HealthIncrement = Cast<ATC_HealthIncrement>(Collectable)) {
    // pick up the collectable
    HealthIncrement->PickUp();
  }
  else if (ATC_Shield* Shield = Cast<ATC_Shield>(Collectable)) {
    // pick up the collectable
    Shield->PickUp();
  }
  else if (ATC_PowerUnit* PowerUnit = Cast<ATC_PowerUnit>(Collectable)) {
    // pick up the collectable
    PowerUnit->PickUp();
    // Update the number of power units, that the character has
    StatusComponent->UpdatePowerUnitsInPossession(1);
  }
  else if (ATC_Coin* Coin = Cast<ATC_Coin>(Collectable)) {
    // pick up the collectable
    Coin->PickUp();
    // Update the number of coins, that the character has
  }
  else if (ATC_MagicGem* MagicGem = Cast<ATC_MagicGem>(Collectable)) {
    // pick up the collectable
    MagicGem->PickUp();
  }
}
#pragma endregion COLLECTABLES

#pragma region ITEMS
void ATC_DonutPlayer::GrabItem(UPrimitiveComponent* ComponentToGrab)
{
  // if player has no item grabbed and the component has the correct tag
  if (!ItemGrabbed && ComponentToGrab->ComponentHasTag("Grab"))
  {
    // Save item to grab
    ItemGrabbed = ComponentToGrab;

    if (AnimInst)
    {
      // Allow grab animation
      ExecuteAnimation("Grab");
    }

    if (DonutPlayerController)
    {
      // disable input for player while grabbing the item
      DonutPlayerController->SetPlayerEnabledState(false, true);
    }

    // Timer to enable the player to grab an item
    FTimerHandle PlayerTimerAttachItem;
    GetWorldTimerManager().SetTimer(PlayerTimerAttachItem, this, &ATC_DonutPlayer::AttachItem, GrabItemDelay, false);
  }
}

/*
* Attach the component to the player and enable input again
*/
void ATC_DonutPlayer::AttachItem()
{
  if (!ItemGrabbed)
    return;

  ItemGrabbed->SetSimulatePhysics(false);
  ItemGrabbed->AttachToComponent(SphereGrabber,
    FAttachmentTransformRules::SnapToTargetNotIncludingScale);
  ItemGrabbed->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
  ItemGrabbed->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
  ItemGrabbed->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Block);
  

  // enable input for player
  DonutPlayerController->SetPlayerEnabledState(true, true);
}

void ATC_DonutPlayer::HandleThrowItem()
{
  if (DonutPlayerController)
  {
    // disable input for player while grabbing the item
    DonutPlayerController->SetPlayerEnabledState(false, true);
  }

  if (AnimInst)
  {
    // Allow throw animation
    ExecuteAnimation("Throw");
  }

  // Timer to enable the player to grab an item
  FTimerHandle PlayerTimerThrowItem;
  GetWorldTimerManager().SetTimer(PlayerTimerThrowItem, this, &ATC_DonutPlayer::ThrowItem, ThrowItemDelay, false);
}

void ATC_DonutPlayer::ThrowItem()
{
  // direction of the forwarvector + slightly variation to the center
  FRotator ThrowRotation = GetActorForwardVector().Rotation() + ThrowDeviation;

  ItemGrabbed->SetSimulatePhysics(true);
  ItemGrabbed->SetPhysicsLinearVelocity((ThrowRotation.Vector() + FVector(0.f, 0.f, 0.30f)) * 1000);
  ItemGrabbed->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
  ItemGrabbed->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
  ItemGrabbed = nullptr;

  // enable input for player
  DonutPlayerController->SetPlayerEnabledState(true, true);
}

/*
* Use the grabbed item to defend
*/
void ATC_DonutPlayer::DefendWithItem()
{
  if (!ItemGrabbed || !DefendAnimMontage)
    return;

  if (!AnimInst->Montage_IsPlaying(DefendAnimMontage))
  {
    ExecuteAnimMontage("Defense");
  }
}

/*
* Stop Defense with item
*/
void ATC_DonutPlayer::StopDefense()
{
  if (!DefendAnimMontage)
    return;

  if (AnimInst->Montage_IsPlaying(DefendAnimMontage))
  {
    StopAnimMontage(DefendAnimMontage);
  }
}
#pragma endregion ITEMS

#pragma region BOMBS
void ATC_DonutPlayer::IncrementBombs(int Number)
{
  NumberOfBombs += Number;
  
  // Update the number of the bombs in the widget
  if (DonutGameMode)
  {
    DonutGameMode->UpdatePlayerBombsWidget();
  }
}

void ATC_DonutPlayer::ReduceBombs()
{
  if (NumberOfBombs <= 0)
  {
    NumberOfBombs = 0;
    return;
  }
  NumberOfBombs--;

  // Update the number of the bombs in the widget
  if (DonutGameMode)
  {
    DonutGameMode->UpdatePlayerBombsWidget();
  }
}

void ATC_DonutPlayer::ThrowBomb()
{
  if (!BombClass || NumberOfBombs <= 0)
    return;

  FVector SpawnLocation = GetActorLocation();
  SpawnLocation.Z = 40.f;
  ATC_Projectile* Bomb = GetWorld()->SpawnActor<ATC_Projectile>(BombClass,
    SpawnLocation,
    FRotator(0.f, 0.f, -30.f));
  Bomb->SetOwner(this);

  ReduceBombs();
}
#pragma endregion BOMBS

#pragma region PROJECTILES
/* PROJECTILES */
void ATC_DonutPlayer::Fire()
{
  if (!bCanFire || !bCharacterActive || !FireProjectileClass || !WaterProjectileClass)
    return;

  if (ItemGrabbed)
  {
    // throw item, but not shoot
    HandleThrowItem();
    return;
  }

  if (!HandleProjectileComponent)
    return;
  HandleProjectileComponent->FireProjectile();
  
}

/*
* Function to create a generic timer
*/
template< class UserClass >
void ATC_DonutPlayer::CreateGenericTimer(UserClass* InObj, 
  typename FTimerDelegate::TMethodPtr<UserClass> InTimerMethod, float Delay)
{
  FTimerHandle GenericTimer;
  GetWorldTimerManager().SetTimer(GenericTimer, InObj, InTimerMethod, Delay, false);
}

void ATC_DonutPlayer::TimerToReloadProjectiles(float Delay, EProjectile ProjectileTypeToLoad)
{
  FTimerHandle ReloadProjectileTimer;
  // we need a timer delegate because the function to call has input parameters
  FTimerDelegate ReloadProjectileDelegate = FTimerDelegate::CreateUObject(
    HandleProjectileComponent,
    &UTC_HandleProjectile::ReloadProjectiles,
    ProjectileTypeToLoad
  );
  GetWorldTimerManager().SetTimer(ReloadProjectileTimer, ReloadProjectileDelegate, Delay, false);
}

/*
* Function executes when the player overlaps a projectile - Reload projectile
* @param ProjectileType: Type of the projectile (fire, water, plant)
* @param IfProjectileHitEnemy: bool to know if the projectile hit enemy, in order to apply a effect
*/
void ATC_DonutPlayer::ReloadProjectilesAfterOverlap(EProjectile ProjectileType, bool IfProjectileHitEnemy)
{
  // Reload the projectile
  HandleProjectileComponent->ReloadProjectiles(ProjectileType);

  /* Check if any effect has to be applied after recovering the projectile */

  // Get the information of the projectile recovered save it
  FUProjectileInformation ProjectileRecovered;
  GetProjectileInformationFromTypeInProjectileComponent(ProjectileType, ProjectileRecovered);

  // if the projectile hit the enemy, add increment of damage for the projectile
  if (IfProjectileHitEnemy)
  {
    if (!StatusComponent)
    {
      UE_LOG(LogTemp, Error, TEXT("StatusComponent NOT CREATED"));
      return;
    }

    // Add effect - get effect stacks for projectile and damage percentage
    StatusComponent->AddSideEffect(ProjectileRecovered);
  }

}

/*
* Use the HANDLEPROJECTILECOMPONENT component to modify the stacks of a projectile
* @param ProjectileRecovered: information of the projectile recovered
* @param ProjectileStacks: number of the stacks to add
* @param ProjectilePercentageDamageIncrement: percentage increment per stack
*/
void ATC_DonutPlayer::UpdateProjectileStacks(FUProjectileInformation ProjectileRecovered,
  int ProjectileStacks, float ProjectilePercentageDamageIncrement)
{
  // Update the stacks and increment of damage of the projectile in the HandleProjectileComponent
  HandleProjectileComponent->UpdateProjectileStackInformation(ProjectileRecovered, ProjectileStacks,
    ProjectilePercentageDamageIncrement);
}

/*
* Use the HANDLEPROJECTILECOMPONENT component to get the specific projectile information
* @param ProjectileType: type of the projectile (fire, water, plant)
* @outparam ProjInformation: information of the projectile
*/
void ATC_DonutPlayer::GetProjectileInformationFromTypeInProjectileComponent(EProjectile ProjectileType,
  FUProjectileInformation& ProjInformation)
{
  HandleProjectileComponent->GetProjectileInformationFromType(ProjectileType, ProjInformation);
}

/* PROJECTILES */
#pragma endregion PROJECTILES

#pragma region ANIMATIONS
/* ANIMATIONS */

void ATC_DonutPlayer::ExecuteAnimation(FString AnimationToExecute)
{
  if (!AnimInst)
    return;

  if (AnimationToExecute == "Throw")
  {
    AnimInst->ThrowAnimation();
  }
  else if (AnimationToExecute == "Grab")
  {
    AnimInst->GrabAnimation();
  }
}

void ATC_DonutPlayer::ExecuteAnimMontage(FString AnimationToExecute)
{
  if (!DefendAnimMontage || !ThrowAnimMontage)
    return;

  if (AnimationToExecute == "Defense")
  {
    PlayAnimMontage(DefendAnimMontage, 0.0f);
  }
  else if (AnimationToExecute == "Throw")
  {
    PlayAnimMontage(ThrowAnimMontage, 1.0f);
  }
}


/* END ANIMATIONS */
#pragma endregion ANIMATIONS

#pragma region BUFFS

void ATC_DonutPlayer::MultipleShoot()
{
  bMultipleShoot = true;
  DonutGameMode->MultipleShootWidget();
}

void ATC_DonutPlayer::IncrementHealth(float HealthIncrementation)
{
  GetHealthComponent()->SetMaxHealth(GetHealthComponent()->GetMaxHealth() + HealthIncrementation);
  GetHealthComponent()->SetHealth(GetHealthComponent()->GetHealth() + HealthIncrementation);
}

void ATC_DonutPlayer::IncrementShield(float ShieldIncrementation)
{
  GetShieldComponent()->SetHealth(GetShieldComponent()->GetHealth() + ShieldIncrementation);
}

/* END BUFF COLLECTABLES */
#pragma endregion BUFFS

#pragma region POWERUNITS

/*
* Function called to add the item bought to the player and pay the resourses
*/
void ATC_DonutPlayer::BuyItemInShop(FText ItemBought, int PowerUnitsToPay, int MoneyToPay)
{
  StatusComponent->UpdateItemsPlayer(ItemBought, PowerUnitsToPay, MoneyToPay);
}

/*
* Return the Power units the player possess for a skin type
*/
int ATC_DonutPlayer::GetPowerUnitsOfType(ESkin SkinType)
{
  switch (SkinType)
  {
  case ESkin::Fire:
    return StatusComponent->GetFireSkinPowerUnits();

  case ESkin::Water:
    return StatusComponent->GetWaterSkinPowerUnits();

  case ESkin::Plant:
    return StatusComponent->GetPlantSkinPowerUnits();
  }
  return 0;
}

#pragma endregion POWERUNITS