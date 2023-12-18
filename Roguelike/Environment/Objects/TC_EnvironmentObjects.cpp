// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_EnvironmentObjects.h"
#include "../../Characters/TC_BaseCharacter.h"
#include "../../Characters/TC_Boss.h"

// Sets default values
ATC_EnvironmentObjects::ATC_EnvironmentObjects()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RoomElements = CreateDefaultSubobject<USceneComponent>(TEXT("Room Elements"));
	RoomElements->SetupAttachment(RootComponent);

	EnemySpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Enemy Spawn Point 1"));
	EnemySpawnPoint->SetupAttachment(RoomElements);
	EnemySpawnPoints.Add(EnemySpawnPoint);

	EnemySpawnPoint2 = CreateDefaultSubobject<USceneComponent>(TEXT("Enemy Spawn Point 2"));
	EnemySpawnPoint2->SetupAttachment(RoomElements);
	EnemySpawnPoints.Add(EnemySpawnPoint2);

	EnemySpawnPoint3 = CreateDefaultSubobject<USceneComponent>(TEXT("Enemy Spawn Point 3"));
	EnemySpawnPoint3->SetupAttachment(RoomElements);
	EnemySpawnPoints.Add(EnemySpawnPoint3);

	EnemySpawnPoint4 = CreateDefaultSubobject<USceneComponent>(TEXT("Enemy Spawn Point 4"));
	EnemySpawnPoint4->SetupAttachment(RoomElements);
	EnemySpawnPoints.Add(EnemySpawnPoint4);
}

// Called when the game starts or when spawned
void ATC_EnvironmentObjects::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATC_EnvironmentObjects::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATC_EnvironmentObjects::Initialize(bool ShouldSpawnBoss, TSubclassOf<ATC_Boss> BossClass, 
	int DungeonTileSize, TArray<TSubclassOf<ATC_BaseCharacter>> ArrayEnemies)
{
	TileSize = DungeonTileSize;
	ArrayOfEnemies = ArrayEnemies;
	if (!ShouldSpawnBoss)
	{
		SetEnemiesForRoom();
	}
	else
	{
		SetBossForRoom(BossClass);
	}
}

/*
* Set random enemies in the room
*/
void ATC_EnvironmentObjects::SetEnemiesForRoom()
{
	for (USceneComponent* SpawnPoint : EnemySpawnPoints)
	{
		// Get a random number to see if enemy is spawned in the spawn point
		int IfEnemySpawns = FMath::RandRange(0, 2);
		int RandomEnemy;
		if (IfEnemySpawns < 2) // if 0 or 1 spawn enemy
		{
			/*FVector EnemySpawn = FVector(SpawnPoint->GetComponentLocation().X * TileSize,
				SpawnPoint->GetComponentLocation().Y * TileSize, 200.f);*/
			FVector EnemySpawn = SpawnPoint->GetComponentLocation();
			EnemySpawn.Z = 200.f;
			RandomEnemy = FMath::RandRange(0, ArrayOfEnemies.Num() - 1);
			ATC_BaseCharacter* Enemy = GetWorld()->SpawnActor<ATC_BaseCharacter>(
					ArrayOfEnemies[RandomEnemy], EnemySpawn, FRotator::ZeroRotator);
				//Enemy->ActivateEnemy(true);
		}
	}
	//int NumberEnemiesInRoom = FMath::RandRange(1, 4);
	//TArray<FVector> EnemyAlreadySpawnPositions;

	//int PosX;
	//int PosY;
	//int RandomEnemy;
	//int DungeonXSize = DungeonRoomSize.X;
	//int DungeonYSize = DungeonRoomSize.Y;
	//for (int i = 0; i < NumberEnemiesInRoom; i++)
	//{
	//	bool EnemyPlaced = false;
	//	// while the enemy was not placed in the room
	//	while (!EnemyPlaced)
	//	{
	//		PosX = FMath::RandRange(0, DungeonXSize - 1);
	//		PosY = FMath::RandRange(0, DungeonYSize - 1);
	//		RandomEnemy = FMath::RandRange(0, ArrayEnemies.Num() - 1);

	//		// if no other enemy was placed in this position
	//		if (EnemyAlreadySpawnPositions.Find(FVector(PosX, PosY, 0.f)) == INDEX_NONE)
	//		{
	//			// Spawn enemy
	//			FVector EnemySpawn = FVector(PosX * DungeonTileSize, PosY * DungeonTileSize, 200.f) +
	//				(DungeonRoomPosition * DungeonTileSize);
	//			ATC_BaseCharacter* Enemy = GetWorld()->SpawnActor<ATC_BaseCharacter>(
	//				ArrayEnemies[RandomEnemy], EnemySpawn, FRotator::ZeroRotator);
	//			//Enemy->ActivateEnemy(true);

	//			// Save enemy position in the array
	//			EnemyAlreadySpawnPositions.Add(FVector(PosX, PosY, 0.f));
	//			EnemyPlaced = true;
	//		}
	//	}
	//}
}

/*
* Set random enemies in the room
*/
void ATC_EnvironmentObjects::SetBossForRoom(TSubclassOf<ATC_Boss> BossClass)
{
	if (!BossClass)
		return;

	ATC_Boss* EnemyBoss = GetWorld()->SpawnActor<ATC_Boss>(BossClass, EnemySpawnPoint->GetComponentLocation(),
		FRotator(0.f, 00.f, 0.f));
	EnemyBoss->SetActorScale3D(FVector(3.f, 3.f, 3.f));
}