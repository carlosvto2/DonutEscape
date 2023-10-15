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
#include "../Actors/TC_Door.h"
#include "../ActorComponents/TC_MoveComponent.h"
#include "../ActorComponents/TC_HealthComponent.h"
#include "../Controllers/TC_DonutPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Actors/TC_Projectile.h"
#include "../TC_GameMode.h"

ATC_DonutPlayer::ATC_DonutPlayer()
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

  // Add shield
  PlayerShield = CreateDefaultSubobject<UTC_HealthComponent>(TEXT("Shield"));

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
  }
}

void ATC_DonutPlayer::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (!DonutPlayerController)
    return;

  FHitResult HitResult;
  // Get the point where the cursor is hitting
  DonutPlayerController->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, HitResult);
  //DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 50.0f, 12, FColor::Blue);

  // rotate the character to the cursor location
  RotateMesh(HitResult.ImpactPoint);


  // Handle if there is any collectable around
  TArray<AActor*> OverlappedActors;
  if (CheckIfOverlappingCollectables(OverlappedActors))
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

void ATC_DonutPlayer::HandleDestruction()
{
  Super::HandleDestruction();
  SetActorHiddenInGame(true);
  SetActorTickEnabled(false);
}


// Called when the game starts or when spawned
void ATC_DonutPlayer::BeginPlay()
{
  Super::BeginPlay();

  DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));
  if (!DonutGameMode)
    return;

  SpringArmActualRotation = SpringArm->GetComponentRotation();
  KeyPicked = false;
  bCharacterActive = true;

  MovementComponent = GetCharacterMovement();

  // player controller
  DonutPlayerController = Cast<APlayerController>(GetController());
  // shield component (Health component)
  PlayerShield->SetComponentAsShield(true);

  //Add Input Mapping Context
  if (DonutPlayerController)
  {
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(DonutPlayerController->GetLocalPlayer()))
    {
      Subsystem->AddMappingContext(MappingContext, 0);
    }
  }
}

/*
* Get the overlapped actors and return true if there is any collectable
*/
bool ATC_DonutPlayer::CheckIfOverlappingCollectables(TArray<AActor*> &OverlappedActors)
{
  // Get all overlapping actors and save them into collectables
  SphereCollector->GetOverlappingActors(OverlappedActors);

  for (AActor* OverlappedActor : OverlappedActors)
  {
    // IF COLLECTABLE
    ATC_Collectable* Collectable = Cast<ATC_Collectable>(OverlappedActor);
    //if the overpped actor is a collectable
    if (Collectable && Collectable->IsActive())
    {
      return true;
    }
  }
  return false;
}

/*
* When press E, the character will pick all the collectors within the spherecollector component
*/
void ATC_DonutPlayer::PickupCollector()
{
  TArray<AActor*> OverlappedActors;
  CheckIfOverlappingCollectables(OverlappedActors);

  for (AActor* OverlappedActor : OverlappedActors)
  {
    // IF COLLECTABLE
    ATC_Collectable* Collectable = Cast<ATC_Collectable>(OverlappedActor);
    //if the overpped actor is a collectable
    if (Collectable && Collectable->IsActive())
    {
      // check collectable to pick
      CastToPossibleCollectables(Collectable);
      return;
    }

    // IF DOOR
    ATC_Door* Door = Cast<ATC_Door>(OverlappedActor);
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
        ATC_DonutPlayerController* DonutController = Cast<ATC_DonutPlayerController>(DonutPlayerController);
        DonutController->SetPlayerEnabledState(false);
        DonutGameMode->GameOver(true);
      }
      return;
    }

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

/*
* Increment overheat everytime player fires
*/
void ATC_DonutPlayer::IncrementOverheat()
{
  Overheat += OverheatIncrementation;
  OverheatReaction();
}

/*
* Decrease overheat
*/
void ATC_DonutPlayer::DecreaseOverheat()
{
  Overheat -= OverheatDecrement;
  if (Overheat < 0.f)
    Overheat = 0.f;
}

/*
* If the overheat reaches the maximum
*/
void ATC_DonutPlayer::OverheatReaction()
{
  if (Overheat < MaxOverheat)
    return;

  bInOverheat = true;
  SetCanFire(false);
  MovementComponent->MaxWalkSpeed = 300.f;
}

/*
* If the overheat was maximum and it recovers
*/
void ATC_DonutPlayer::RecoverFromOverheat()
{
  bInOverheat = false;
  SetCanFire(true);
  MovementComponent->MaxWalkSpeed = 600.f;
}

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

/* BUFF COLLECTABLES */

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