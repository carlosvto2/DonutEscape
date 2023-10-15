// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_Room.h"
#include "TC_SpawnPoint.h"
#include "TC_DoorCollision.h"
#include "../Characters/TC_BaseCharacter.h"
#include "../Characters/TC_ChaseEnemy.h"
#include "../Characters/TC_Boss.h"
#include "../Actors/Collectables/TC_MultipleShoot.h"
#include "../Actors/TC_Door.h"
#include "../TC_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "../Actors/Collectables/TC_Collectable.h"

// Sets default values
ATC_Room::ATC_Room()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RoomElements = CreateDefaultSubobject<USceneComponent>(TEXT("RoomElements"));
	RoomElements->SetupAttachment(RootComponent);
	RoomElements->SetMobility(EComponentMobility::Static);

	// Create and set the spawnpoints
	SetRoomSpawnPoints();

	// Create colliders
	SetRoomColliders();

	// Create enemy spawnPoints
	SetEnemySpawnPoints();

	// Initialize the FName material array
	SetWallsMaterialArray();
}

void ATC_Room::SetRoomSpawnPoints()
{
	BottomRoomSpawnPoint = CreateDefaultSubobject<UTC_SpawnPoint>(TEXT("Bottom Room Spawn Point"));
	BottomRoomSpawnPoint->SetupAttachment(RootComponent);
	BottomRoomSpawnPoint->SetRelativeLocation(FVector(-2000.f, 0.f, 90.f));
	BottomRoomSpawnPoint->SetMobility(EComponentMobility::Static);
	BottomRoomSpawnPoint->SetRoomOwner(this);
	SpawnPoints.Add(BottomRoomSpawnPoint);

	TopRoomSpawnPoint = CreateDefaultSubobject<UTC_SpawnPoint>(TEXT("Top Room Spawn Point"));
	TopRoomSpawnPoint->SetupAttachment(RootComponent);
	TopRoomSpawnPoint->SetRelativeLocation(FVector(2000.f, 0.f, 90.f));
	TopRoomSpawnPoint->SetMobility(EComponentMobility::Static);
	TopRoomSpawnPoint->SetRoomOwner(this);
	SpawnPoints.Add(TopRoomSpawnPoint);

	RightRoomSpawnPoint = CreateDefaultSubobject<UTC_SpawnPoint>(TEXT("Right Room Spawn Point"));
	RightRoomSpawnPoint->SetupAttachment(RootComponent);
	RightRoomSpawnPoint->SetRelativeLocation(FVector(0.f, 2000.f, 90.f));
	RightRoomSpawnPoint->SetMobility(EComponentMobility::Static);
	RightRoomSpawnPoint->SetRoomOwner(this);
	SpawnPoints.Add(RightRoomSpawnPoint);

	LeftRoomSpawnPoint = CreateDefaultSubobject<UTC_SpawnPoint>(TEXT("Left Room Spawn Point"));
	LeftRoomSpawnPoint->SetupAttachment(RootComponent);
	LeftRoomSpawnPoint->SetRelativeLocation(FVector(0.f, -2000.f, 90.f));
	LeftRoomSpawnPoint->SetMobility(EComponentMobility::Static);
	LeftRoomSpawnPoint->SetRoomOwner(this);
	SpawnPoints.Add(LeftRoomSpawnPoint);


	SpawnPointInitialRoom = CreateDefaultSubobject<UTC_SpawnPoint>(TEXT("Initial Room Spawn Point"));
	SpawnPointInitialRoom->SetRelativeLocation(FVector(0.f, 0.f, 90.f)); // relative location
	SpawnPointInitialRoom->SetRoomOwner(this);
}

void ATC_Room::SetRoomColliders()
{
	BottomRoomCollision = CreateDefaultSubobject<UTC_DoorCollision>(TEXT("Bottom Room Collision"));
	BottomRoomCollision->SetupAttachment(RoomElements);
	BottomRoomCollision->SetBoxExtent(FVector(50.f, 50.f, 200.f)); // dimensions
	BottomRoomCollision->SetRelativeLocation(FVector(-950.f, 0.f, 90.f)); // relative location
	BottomRoomCollision->SetMobility(EComponentMobility::Static); // Movility static
	BottomRoomCollision->SetPositionInRoom(ECollPosition::Bottom); // Set position in room
	BottomRoomCollision->SetRoomOwner(this);
	RoomColliders.Add(BottomRoomCollision);

	TopRoomCollision = CreateDefaultSubobject<UTC_DoorCollision>(TEXT("Top Room Collision"));
	TopRoomCollision->SetupAttachment(RoomElements);
	TopRoomCollision->SetBoxExtent(FVector(50.f, 50.f, 200.f));
	TopRoomCollision->SetRelativeLocation(FVector(950.f, 0.f, 90.f));
	TopRoomCollision->SetMobility(EComponentMobility::Static);
	TopRoomCollision->SetPositionInRoom(ECollPosition::Top);
	TopRoomCollision->SetRoomOwner(this);
	RoomColliders.Add(TopRoomCollision);

	RightRoomCollision = CreateDefaultSubobject<UTC_DoorCollision>(TEXT("Right Room Collision"));
	RightRoomCollision->SetupAttachment(RoomElements);
	RightRoomCollision->SetBoxExtent(FVector(50.f, 50.f, 200.f));
	RightRoomCollision->SetRelativeLocation(FVector(0.f, 950.f, 90.f));
	RightRoomCollision->SetMobility(EComponentMobility::Static);
	RightRoomCollision->SetPositionInRoom(ECollPosition::Right);
	RightRoomCollision->SetRoomOwner(this);
	RoomColliders.Add(RightRoomCollision);

	LeftRoomCollision = CreateDefaultSubobject<UTC_DoorCollision>(TEXT("Left Room Collision"));
	LeftRoomCollision->SetupAttachment(RoomElements);
	LeftRoomCollision->SetBoxExtent(FVector(50.f, 50.f, 200.f));
	LeftRoomCollision->SetRelativeLocation(FVector(0.f, -950.f, 90.f));
	LeftRoomCollision->SetMobility(EComponentMobility::Static);
	LeftRoomCollision->SetPositionInRoom(ECollPosition::Left);
	LeftRoomCollision->SetRoomOwner(this);
	RoomColliders.Add(LeftRoomCollision);
}

void ATC_Room::SetEnemySpawnPoints()
{
	EnemySpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Enemy Spawn Point 1"));
	EnemySpawnPoint->SetupAttachment(RootComponent);
	EnemySpawnPoints.Add(EnemySpawnPoint);

	EnemySpawnPoint2 = CreateDefaultSubobject<USceneComponent>(TEXT("Enemy Spawn Point 2"));
	EnemySpawnPoint2->SetupAttachment(RootComponent);
	EnemySpawnPoints.Add(EnemySpawnPoint2);

	EnemySpawnPoint3 = CreateDefaultSubobject<USceneComponent>(TEXT("Enemy Spawn Point 3"));
	EnemySpawnPoint3->SetupAttachment(RootComponent);
	EnemySpawnPoints.Add(EnemySpawnPoint3);

	EnemySpawnPoint4 = CreateDefaultSubobject<USceneComponent>(TEXT("Enemy Spawn Point 4"));
	EnemySpawnPoint4->SetupAttachment(RootComponent);
	EnemySpawnPoints.Add(EnemySpawnPoint4);
}

/*
* This array will have the tag of the walls that need to be transparent
*/
void ATC_Room::SetWallsMaterialArray()
{
	TransparentWallMaterials.Add("Bottom");
	TransparentWallMaterials.Add("Bottom Left");
	TransparentWallMaterials.Add("Left");
	TransparentWallMaterials.Add("Left Top");
	TransparentWallMaterials.Add("Top");
	TransparentWallMaterials.Add("Top Right");
	TransparentWallMaterials.Add("Right");
	TransparentWallMaterials.Add("Right Bottom");
}

/*
* Get the next tags to make transparent
*/
void ATC_Room::GetArrayOfTags(TArray<FName>& ArrayTags, int TransparentWallMovement)
{
	// Get next part with transparent walls
	PartWallsWithTransparentMaterial += TransparentWallMovement;
	if (PartWallsWithTransparentMaterial >= TransparentWallMaterials.Num())
	{
		// restart array if limit reached
		PartWallsWithTransparentMaterial -= 8;
	}

	// the actual rotation can have two tags
	FString StringTags = TransparentWallMaterials[PartWallsWithTransparentMaterial].ToString();
	TArray<FString> ActualTags;
	StringTags.ParseIntoArray(ActualTags, TEXT(" "));

	ArrayTags = TArray<FName>(ActualTags);
}

/*
* Make transparent the correspondant walls
*/
void ATC_Room::ChangeWallMaterialsAfterRotation(USoundWave* MoveRoomSound, int TransparentWallMovement)
{
	if (MoveRoomSound)
	{
		UGameplayStatics::PlaySound2D(this, MoveRoomSound, 0.05f, 1.3f);
	}

	// Get the tags of the transparent walls
	TArray<FName> ActualTags;
	GetArrayOfTags(ActualTags, TransparentWallMovement);

	// Get all meshes with the appropiate tag
	TArray<UStaticMeshComponent*> Meshes;
	GetComponents<UStaticMeshComponent>(Meshes);


	// Check meshes in the room
	for (UStaticMeshComponent* Mesh : Meshes)
	{
		// if it has no tags
		if (!Mesh || Mesh->ComponentTags.Num() == 0)
			return;

		// check if the any wall has these tags
		for (FName ActualTag : ActualTags)
		{
			// if it has one of the actual tag to use
			if (Mesh->ComponentHasTag(ActualTag))
			{
				// IT HAS THE TAG
				// Change material to transparent
				// if wall
				if (Mesh->GetMaterial(0) == WallMaterial)
				{
					Mesh->SetMaterial(0, WallMaterialTransparent);
					Mesh->SetMaterial(5, WallMaterialTransparent);
					Mesh->SetMaterial(6, WallMaterialTransparent);
				}
				else if (Mesh->GetMaterial(0) == WindowMaterial)
				{
					// if window
					Mesh->SetMaterial(0, WindowMaterialTransparent);
				}
				break;
			}
			else
			{
				// IT DOES NOT HAVE THE TAG
				// Change material to normal
				// if wall
				if (Mesh->GetMaterial(0) == WallMaterialTransparent)
				{
					Mesh->SetMaterial(0, WallMaterial);
					Mesh->SetMaterial(5, WallMaterial);
					Mesh->SetMaterial(6, WallMaterial);
				}
				else if (Mesh->GetMaterial(0) == WindowMaterialTransparent)
				{
					// if window
					Mesh->SetMaterial(0, WindowMaterial);
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void ATC_Room::BeginPlay()
{
	Super::BeginPlay();
	AllEnemiesKilled = false;
	PartWallsWithTransparentMaterial = 0; // bottom has transparent materials at the beginning

	DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!DonutGameMode)
		return;
}

// Called every frame
void ATC_Room::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATC_Room::HideRoomAndElements(bool bShow)
{
	// show room in game
	SetActorHiddenInGame(bShow);
	// if any final door in the room, show it
	if (FinalDoor)
	{
		FinalDoor->SetActorHiddenInGame(bShow);
	}

	// if boss show/hide it
	if (FinalBossSet && FinalBoss)
	{
		FinalBoss->SetActorHiddenInGame(bShow);
		// show the boss health in the widget
		DonutGameMode->UpdateBossHealthWidget(!bShow, FinalBoss);
	}

	// if buff show/hide it
	if (Buff)
	{
		Buff->SetActorHiddenInGame(bShow);
	}
}

void ATC_Room::SpawnRoomElements(TArray<TSubclassOf<ATC_BaseCharacter>> EnemiesArray, TArray<TSubclassOf<ATC_Collectable>> Collectables)
{
	// if the player killed all the enemies in the room OR FINAL BOSS, they will not spawn again
	if (GetIfAllEnemiesKilled())
		return;

	if (FinalBossSet)
	{
		FinalBoss->ActivateEnemy(true);
		return;
	}

	for (USceneComponent* SpawnPoint : GetEnemySpawnPoints())
	{
		TryToSpawnEnemiesAndActivate(EnemiesArray, SpawnPoint);
		TryToSpawnCollectable(Collectables, SpawnPoint);
	}
}

void ATC_Room::TryToSpawnCollectable(TArray<TSubclassOf<ATC_Collectable>> Collectables, USceneComponent* SpawnPoint)
{
	int RandomCollectable;
	// also try to spawn collectable
		// Get a random number to see if collectable is spawned in the spawn point
	RandomCollectable = FMath::RandRange(0, 9);
	if (RandomCollectable < 1) // if 0 spawn collectable
	{
		RandomCollectable = FMath::RandRange(0, Collectables.Num() - 1);
		if (!Collectables[RandomCollectable])
			return;

		FVector CollectableSpawn = GetActorLocation() + SpawnPoint->GetComponentLocation() + FVector(0.f, 0.f, 20.f);
		GetWorld()->SpawnActor<ATC_Collectable>(Collectables[RandomCollectable],
			CollectableSpawn, FRotator(0.f, 0.f, 0.f));
	}
}

void ATC_Room::TryToSpawnEnemiesAndActivate(TArray<TSubclassOf<ATC_BaseCharacter>> EnemiesArray, USceneComponent* SpawnPoint)
{
	int RandomEnemy;

	// Get a random number to see if enemy is spawned in the spawn point
	RandomEnemy = FMath::RandRange(0, 2);
	if (RandomEnemy < 2) // if 0 or 1 spawn enemy
	{
		// another random number to get a random enemy
		RandomEnemy = FMath::RandRange(0, EnemiesArray.Num() - 1);
		FVector EnemySpawn = GetActorLocation() + SpawnPoint->GetComponentLocation() + FVector(0.f, 0.f, 20.f);

		ATC_BaseCharacter* Enemy = GetWorld()->SpawnActor<ATC_BaseCharacter>(
			EnemiesArray[RandomEnemy], EnemySpawn, FRotator::ZeroRotator);

		if (!Enemy)
			return;
		// Save a pointer to this room in the enemy
		Enemy->SetOwnerRoom(this);
		Enemies.Add(Enemy);

		// Activate Enemy
		Enemy->ActivateEnemy(true);
	}
}

void ATC_Room::DeleteAllEnemiesInRoom()
{
	// if boss exist, just deactivate
	if (FinalBossSet && FinalBoss)
	{
		FinalBoss->ActivateEnemy(false);
		return;
	}
	for (ATC_BaseCharacter* Enemy : Enemies)
	{
		Enemy->Destroy();
	}
	Enemies.Empty();
}

/*
* Check if last enemy in the room
*/
void ATC_Room::CheckIfLastEnemy(ATC_BaseCharacter* EnemyKilled)
{
	if (!EnemyKilled)
		return;


	Enemies.Remove(EnemyKilled);

	// If no more enemies, player killed all enemies in room
	if (Enemies.Num() == 0)
	{
		AllEnemiesKilled = true;
	}
}

void ATC_Room::CreateBuff(TSubclassOf<ATC_MultipleShoot> BuffClass)
{
	if (!BuffClass)
		return;

	int RandomSpawnPoint;
	RandomSpawnPoint = FMath::RandRange(0, EnemySpawnPoints.Num() - 1);
	FVector BuffSpawn = GetActorLocation() + EnemySpawnPoints[RandomSpawnPoint]->GetComponentLocation() + FVector(0.f, 0.f, 20.f);
	Buff = GetWorld()->SpawnActor<ATC_MultipleShoot>(BuffClass, BuffSpawn, FRotator(0.f, 0.f, 0.f));
	Buff->SetOwner(this);
}

void ATC_Room::SaveFinalBoss(ATC_Boss* Boss)
{
	FinalBoss = Boss;
	FinalBossSet = true;
}

void ATC_Room::DeactivateSpawnPointForRoom(int SpawnPointPosition)
{
	if (SpawnPointPosition == 1)
	{
		BottomRoomSpawnPoint->SetNeedToSpawn(false);
	}
	else if (SpawnPointPosition == 2)
	{
		TopRoomSpawnPoint->SetNeedToSpawn(false);
	}
	else if (SpawnPointPosition == 3)
	{
		LeftRoomSpawnPoint->SetNeedToSpawn(false);
	}
	else if (SpawnPointPosition == 4)
	{
		RightRoomSpawnPoint->SetNeedToSpawn(false);
	}
}
