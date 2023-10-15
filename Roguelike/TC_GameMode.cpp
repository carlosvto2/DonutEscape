// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Characters/TC_DonutPlayer.h"
#include "Controllers/TC_DonutPlayerController.h"
#include "Characters/TC_EnemyTurret.h"
#include "Characters/TC_ChaseEnemy.h"
#include "Environment/TC_Room.h"
#include "Environment/TC_SpawnPoint.h"
#include "Math/UnrealMathUtility.h"
#include "Actors/TC_Door.h"
#include "Environment/TC_DoorCollision.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Actors/Collectables/TC_MagicBox.h"
#include "Actors/Collectables/TC_MultipleShoot.h"
#include "Characters/TC_Boss.h"

// This will rotate the whole map
void ATC_GameMode::HandleMapRotation()
{
  if (TurnDegrees.Num() == 0)
    return;

  // Amount of degrees to rotate the player
  int RandomNumber = FMath::RandRange(1, TurnDegrees.Num());
  RandomRotationAndWallTransparency += RandomNumber;
  if (RandomRotationAndWallTransparency > TurnDegrees.Num())
  {
    // restart array if limit reached
    RandomRotationAndWallTransparency -= 7;
  }

  // rotate character the correspondant degrees
  Donut->RotateCharacter(TurnDegrees[RandomNumber - 1]);

  for (ATC_Room* Room : RoomArray)
  {
    if (!Room)
      return;

    Room->ChangeWallMaterialsAfterRotation(MoveRoomsSound, RandomNumber);
  }
}

void ATC_GameMode::ActorDied(AActor* DeadActor)
{
  // if the dead actor is the player
  if (DeadActor == Donut)
  {
    Donut->HandleDestruction();

    if (!DonutPlayerController)
      return;
    // use controller to disable player
    DonutPlayerController->SetPlayerEnabledState(false);

    // End the game
    GameOver(false);
  }
  else if(ATC_EnemyTurret* EnemyTurret = Cast<ATC_EnemyTurret>(DeadActor))
  {
    EnemyTurret->HandleDestruction();
  }
  else if (ATC_ChaseEnemy* ChaseEnemy = Cast<ATC_ChaseEnemy>(DeadActor))
  {
    ChaseEnemy->HandleDestruction();
  }
  else if (ATC_Boss* FinalBoss = Cast<ATC_Boss>(DeadActor))
  {
    FinalBoss->HandleDestruction();
  }
}

/*
* Get the position to move and move player
* @param ActorOverlapped: get the actor which overlapped the collider
* @param DoorCollision: Collider overlapped
*/
void ATC_GameMode::MovePlayerToRoom(AActor* ActorOverlapped, UTC_DoorCollision* DoorCollision)
{
  // Get Connected Collision
  UTC_DoorCollision* ConnectedDoorCollision = DoorCollision->GetConnectedCollision();

  ATC_DonutPlayer* DonutPlayer = Cast<ATC_DonutPlayer>(ActorOverlapped);
  if (!DonutPlayer || DonutPlayerController->GetIfPlayerAutomaticallyMoving() || !ConnectedDoorCollision)
    return;
  // if it is the player/player not already moving, continue

  FVector PositionPlayerSpawn;
  if (ConnectedDoorCollision->GetPositionInRoom() == ECollPosition::Bottom)
  {
    PositionPlayerSpawn = ConnectedDoorCollision->GetComponentLocation() + BottomPlayerOffset;
  }
  else if(ConnectedDoorCollision->GetPositionInRoom() == ECollPosition::Top)
  {
    PositionPlayerSpawn = ConnectedDoorCollision->GetComponentLocation() + TopPlayerOffset;
  }
  else if (ConnectedDoorCollision->GetPositionInRoom() == ECollPosition::Right)
  {
    PositionPlayerSpawn = ConnectedDoorCollision->GetComponentLocation() + RightPlayerOffset;
  }
  else if (ConnectedDoorCollision->GetPositionInRoom() == ECollPosition::Left)
  {
    PositionPlayerSpawn = ConnectedDoorCollision->GetComponentLocation() + LeftPlayerOffset;
  }

  // Fade in black screen
  BlackScreenFadeInOut(true);
  // Move player to position
  DonutPlayerController->Move(PositionPlayerSpawn);

  // Room that must be hidden
  TArray<ATC_Room*> HideRoomsArray;
  HideRoomsArray.Add(DoorCollision->GetRoomOwner());


  // Timer to enable the player input when the movement finishes
  FTimerHandle PlayerTimerMoveFinished;
  // we need a timer delegate because the function to call has input parameters
  FTimerDelegate PlayerTimerMoveFinishedDelegate = FTimerDelegate::CreateUObject(
    this,
    &ATC_GameMode::HandlePlayerRoomsWhenEnterNewRoom,
    ConnectedDoorCollision->GetRoomOwner(),
    HideRoomsArray
  );
  GetWorldTimerManager().SetTimer(PlayerTimerMoveFinished, PlayerTimerMoveFinishedDelegate, MovementDelay, false);
}


/*
* Generate enemies every time player enters the room
*/
void ATC_GameMode::HandleEnemiesInRoom(ATC_Room* NewRoom, ATC_Room* OldRoom)
{
  if (!NewRoom || EnemiesArray.Num() == 0)
    return;

  NewRoom->SpawnRoomElements(EnemiesArray, Collectables);
  OldRoom->DeleteAllEnemiesInRoom();
}



/*
* function to activate the player after moving and handle rooms TIMER
* @param RoomToShow: room to show
* @param RoomsToHide: rooms to hide
*/
void ATC_GameMode::HandlePlayerRoomsWhenEnterNewRoom(ATC_Room* RoomToShow, TArray<ATC_Room*> RoomsToHide)
{
  DonutPlayerController->SetPlayerEnabledState(true);
  BlackScreenFadeInOut(false);

  // show next room and hide the rest of rooms
  HandleRoomVisibility(RoomToShow, RoomsToHide);

  // Generate enemies and activate them in next room
  HandleEnemiesInRoom(RoomToShow, RoomsToHide[0]);
}


/*
* Show a room and hide the rooms in Array
* @param RoomToShow: room to show
* @param RoomsToHide: rooms to hide
*/
void ATC_GameMode::HandleRoomVisibility(ATC_Room* RoomToShow, TArray<ATC_Room*> RoomsToHide)
{
  // hide all the rooms in array
  for (ATC_Room* RoomToHide : RoomsToHide)
  {
    if (!RoomToHide)
      return;

    RoomToHide->HideRoomAndElements(true);
  }

  if (!RoomToShow)
    return;

  // show room in game
  RoomToShow->HideRoomAndElements(false);
}


/* Handles the player overheat when he shoots */
void ATC_GameMode::HandlePlayerOverheat()
{
  // if the timer is running, stop it
  if (GetWorldTimerManager().IsTimerActive(PlayerOverheatTimerHandle))
  {
    GetWorldTimerManager().ClearTimer(PlayerOverheatTimerHandle);
  }
  // Show the overheat in the widget
  ModifyPlayerOverheatWidget(Donut->GetOverheat(), Donut->GetMaxOverheat(), Donut->GetIfPlayerInOverheat());

  // Set timer to decrease overheat
  GetWorldTimerManager().SetTimer(PlayerOverheatTimerHandle, this, 
    &ATC_GameMode::DecreasePlayerOverheat, OverheatUpdateDelay, true);
}

/*
* This function will be executed with a timer to decrease the playerOverheat
*/
void ATC_GameMode::DecreasePlayerOverheat()
{
  // If no overheat stop timer
  if (Donut->GetOverheat() <= 0)
  {
    if (!Donut->GetCanFire())
    {
      Donut->RecoverFromOverheat();
    }
    GetWorldTimerManager().ClearTimer(PlayerOverheatTimerHandle);
  }
  // Decrease overheat and show it in the widget
  Donut->DecreaseOverheat();
  ModifyPlayerOverheatWidget(Donut->GetOverheat(), Donut->GetMaxOverheat(), Donut->GetIfPlayerInOverheat());
}

void ATC_GameMode::BeginPlay()
{
  Super::BeginPlay();
  SetRooms();
  SetFinalDoor();
  SetMultipleShootBuff();
  SetBoss();
  ConnectRoomColliders();

  HandleGameStart();

  // Show initial room and hide the rest
  HandleRoomVisibility(InitialRoom, RoomArray);

  // Initialize degrees array to move the dungeon
  InitializeRotationDegreesArray();
}

void ATC_GameMode::InitializeRotationDegreesArray()
{
  TurnDegrees.Add(45.f);
  TurnDegrees.Add(90.f);
  TurnDegrees.Add(135.f);
  TurnDegrees.Add(180.f);
  TurnDegrees.Add(225.f);
  TurnDegrees.Add(270.f);
  TurnDegrees.Add(315.f);
}

void ATC_GameMode::HandleGameStart()
{
  if (GameSound)
  {
    UGameplayStatics::PlaySound2D(this, GameSound, 0.8f, 1.0f);
  }
  // Get the player
  Donut = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
  // Get the player controller
  DonutPlayerController = Cast<ATC_DonutPlayerController>(UGameplayStatics::GetPlayerController(this, 0));

  // call start game function, that will be implemented in blueprints
  StartGame();
  BlackScreenFadeInOut(false);

  if (DonutPlayerController)
  {
    // disable player input until the game starts
    DonutPlayerController->SetPlayerEnabledState(false);

    // Timer to enable the player input when the time comes
    FTimerHandle PlayerEnableTimerHandle;
    // we need a timer delegate because the function to call has input parameters
    FTimerDelegate PlayerEnableTimerDelegate = FTimerDelegate::CreateUObject(
      DonutPlayerController,
      &ATC_DonutPlayerController::SetPlayerEnabledState,
      true
    );
    GetWorldTimerManager().SetTimer(PlayerEnableTimerHandle, PlayerEnableTimerDelegate, StartDelay, false);
  }
}

void ATC_GameMode::SetFinalDoor()
{
  // Get last spawned room
  ATC_Room* LastRoom = RoomArray[RoomArray.Num() - 1];
  if (!LastRoom || !DoorClass)
    return;

  FVector PositionToSetDoor = FVector(LastRoom->GetActorLocation() + FVector(905.f, 500.f, -60.f));
  LastDoor = GetWorld()->SpawnActor<ATC_Door>(DoorClass, PositionToSetDoor, FRotator(0.f, 90.f, 0.f));
  LastDoor->SetActorScale3D(FVector(1.5f, 1.5f, 1.5f));
  LastDoor->SetActorHiddenInGame(false);
  // Save the final door in the room, where it was generated
  LastRoom->SetFinalDoor(LastDoor);
}

void ATC_GameMode::SetMultipleShootBuff()
{
  if (!BuffClass)
    return;
  bool BuffSet = false;
  // Get a random room
  int RandomRoom;
  ATC_Room* Room;
  while (!BuffSet)
  {
    RandomRoom = FMath::RandRange(0, RoomArray.Num() - 1);
    Room = RoomArray[RandomRoom];

    // if it is the initial room, or it has the final door continue
    if (Room == InitialRoom)
      continue;


    Room->CreateBuff(BuffClass);
    BuffSet = true;
  }
}

void ATC_GameMode::SetBoss()
{
  bool BossSet = false;
  // Get a random room
  int RandomRoom;
  ATC_Room* Room;
  while(!BossSet)
  {
    RandomRoom = FMath::RandRange(0, RoomArray.Num() - 1);
    Room = RoomArray[RandomRoom];

    // if it is the initial room, or it has the final door continue
    if (Room == InitialRoom || Room->GetIfHasFinalDoor() || !BossClass)
      continue;

    EnemyBoss = GetWorld()->SpawnActor<ATC_Boss>(BossClass, Room->GetActorLocation(), FRotator(0.f, 00.f, 0.f));
    EnemyBoss->SetActorScale3D(FVector(3.f, 3.f, 3.f));
    EnemyBoss->SetActorHiddenInGame(false);
    Room->SaveFinalBoss(EnemyBoss);
    EnemyBoss->SetOwnerRoom(Room);
    BossSet = true;
  }
}


/* 
  Function to set all the rooms for the map
*/
void ATC_GameMode::SetRooms()
{
  if (!InitialRoomClass)
    return;

  // Set initial room
  InitialRoom = GetWorld()->SpawnActor<ATC_Room>(InitialRoomClass, FVector(0.f, 0.f, 90.f), FRotator::ZeroRotator);
  RoomArray.Add(InitialRoom);
  if (!InitialRoom)
    return;

  if (InitialRoom->SpawnPointInitialRoom)
  {
    InitialRoom->SpawnPointInitialRoom->SetNeedToSpawn(false);
    SpawnPointArray.Add(InitialRoom->SpawnPointInitialRoom);
  }

  SetSideRooms(InitialRoom);
}

/*
  Function to rest of the side rooms
*/
void ATC_GameMode::SetSideRooms(ATC_Room* ActualRoom)
{
  if (!ActualRoom)
    return;

  // Get the spawnPoints of the initialRoom
  TArray<UTC_SpawnPoint*> RoomSpawnPoints = ActualRoom->GetSpawnPoints();

  for (UTC_SpawnPoint* SpawnPoint : RoomSpawnPoints)
  {
    // if spawnpoint exists and it needs to spawn a room
    if (SpawnPoint && CheckIfRoomAlreadyInSpawnpoint(SpawnPoint))
    {
      // Spawn room in the spawnpoint
      SpawnRoom(ActualRoom, SpawnPoint);
    }
  }
}


void ATC_GameMode::SpawnRoom(ATC_Room* ActualRoom, UTC_SpawnPoint* RoomSpawnPoint)
{
  ATC_Room* NewRoom;
  int DirectionOfRoomDoor = RoomSpawnPoint->GetOpeningDirection();
  TArray<TSubclassOf<ATC_Room>> RoomsToUse;

  if (DirectionOfRoomDoor == 1)
  {
    RoomsToUse = BottomRooms;
  }
  else if (DirectionOfRoomDoor == 2)
  {
    RoomsToUse = TopRooms;
  }
  else if (DirectionOfRoomDoor == 3)
  {
    RoomsToUse = LeftRooms;
  }
  else if (DirectionOfRoomDoor == 4)
  {
    RoomsToUse = RightRooms;
  }

  // Get a random room
  int RandomNumber = FMath::RandRange(0, RoomsToUse.Num() - 1);

  //if the room to use does not exist
  if (RoomsToUse.Num() == 0)
    return;

  // Get the location of the spawnpoint of the room in the world
  FVector ActualRoomLocation = ActualRoom->GetActorLocation();
  ActualRoomLocation.Z = 0.f;
  FVector SpawnPointWorldLocation = ActualRoomLocation + RoomSpawnPoint->GetComponentLocation();

  // Spawn new room in the spawn location
  NewRoom = GetWorld()->SpawnActor<ATC_Room>(RoomsToUse[RandomNumber], SpawnPointWorldLocation, FRotator::ZeroRotator);

  // Save new room in array of rooms
  RoomArray.Add(NewRoom);

  // Deactivate the spawnpoint of the new room that CONNECTS TO THE ACTUAL ROOM
  NewRoom->DeactivateSpawnPointForRoom(DirectionOfRoomDoor);

  // Create all the side rooms for the New room
  SetSideRooms(NewRoom);
}

// function to check if another room already exists
bool ATC_GameMode::CheckIfRoomAlreadyInSpawnpoint(UTC_SpawnPoint* SpawnPointToCheck)
{
  // No owner or it does not need to spawn a room or it has no opening direction
  if (!SpawnPointToCheck || !SpawnPointToCheck->GetRoomOwner() ||
    !SpawnPointToCheck->GetNeedToSpawn() || SpawnPointToCheck->GetOpeningDirection() == 0)
    return false;
    

  // Get the location of the spawnpoint of the room in the world
  FVector ActualOwnerLocation = SpawnPointToCheck->GetRoomOwner()->GetActorLocation();
  ActualOwnerLocation.Z = 0.f;
  FVector SpawnPointWorldLocation = ActualOwnerLocation
    + SpawnPointToCheck->GetComponentLocation();
  
  
  FVector OtherSpawnPointWorldLocation;
  FVector ActualOtherOwnerLocation;
  for (UTC_SpawnPoint* SpawnPoint : SpawnPointArray)
  {
    
    // get position in world for other spawnpoint
    ActualOtherOwnerLocation = SpawnPoint->GetRoomOwner()->GetActorLocation();
    ActualOtherOwnerLocation.Z = 0.f;
    OtherSpawnPointWorldLocation = ActualOtherOwnerLocation
      + SpawnPoint->GetComponentLocation();
    double Distance = FVector::Distance(SpawnPointWorldLocation, OtherSpawnPointWorldLocation);

    
    // if distance between them is less than 100, assume they are in the same position
    // if spawnpoint already set a room
    if ((Distance < 500.f && !SpawnPoint->GetNeedToSpawn()))
    {
      SpawnPointToCheck->SetNeedToSpawn(false);
      return false;
    }
  }

  // Deactivate the used spawnpoint for the actual room
  SpawnPointToCheck->SetNeedToSpawn(false);
  SpawnPointArray.Add(SpawnPointToCheck);
  return true;
}

void ATC_GameMode::ConnectRoomColliders()
{
  // Save all the colliders in an array
  SaveCollidersArray();

  FVector ColliderLocation;
  FVector ColliderToCompareLocation;
  // Connect colliders to each other
  for (UTC_DoorCollision* Collider : DoorCollisionArray)
  {
    // get position in world for Collider (collision component)
    ColliderLocation = Collider->GetComponentLocation();

    for (UTC_DoorCollision* ColliderToCompare : DoorCollisionArray)
    {
      // continue if colliders from same room
      if (Collider->GetOwner() == ColliderToCompare->GetOwner())
        continue;

      // get position in world for ColliderTocompare (collision component)
      ColliderToCompareLocation = ColliderToCompare->GetComponentLocation();

      // Get Distance between two colliders
      double Distance = FVector::Distance(ColliderLocation, ColliderToCompareLocation);
      // if they are close connect them
      if (Distance < 500.f)
      {
        Collider->SetConnectedCollision(ColliderToCompare);
        ColliderToCompare->SetConnectedCollision(Collider);
        break;
      }
    }
  }
  SetObjectInAllUnusedActivatedColliders();
}

void ATC_GameMode::SaveCollidersArray()
{
  for (ATC_Room* Room : RoomArray)
  {
    if (!Room)
      return;
    // Save all the colliders in an array
    TArray<UTC_DoorCollision*> RoomColliders = Room->GetRoomColliders();
    for (UTC_DoorCollision* Collider : RoomColliders)
    {
      // If the collider exists and is active
      if(Collider && Collider->GetActiveCollider())
        DoorCollisionArray.Add(Collider);
    }
  }
}

/*
* Set a Column if the collider was active, but it will not be used
*/
void ATC_GameMode::SetObjectInAllUnusedActivatedColliders()
{
  if (!MagicBoxClass)
    return;

  for (UTC_DoorCollision* Collider : DoorCollisionArray)
  {
    // if active, but no connected collision
    if (Collider->GetActiveCollider() && !Collider->GetConnectedCollision())
    {
      // Generate magic box in the collider position
      ATC_MagicBox* MagicBox = GetWorld()->SpawnActor<ATC_MagicBox>(MagicBoxClass, 
        Collider->GetComponentLocation(), FRotator::ZeroRotator);

      MagicBox->SetActorScale3D(FVector(1.5f, 1.5f, 3.0f));
    }
  }
}