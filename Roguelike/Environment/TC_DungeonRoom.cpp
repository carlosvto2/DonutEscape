// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_DungeonRoom.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "../TC_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "../Characters/TC_BaseCharacter.h"
#include "Objects/TC_EnvironmentObjects.h"
#include "../Characters/TC_Boss.h"
#include "../Actors/TC_Door.h"

// Sets default values
ATC_DungeonRoom::ATC_DungeonRoom()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

// Called when the game starts or when spawned
void ATC_DungeonRoom::BeginPlay()
{
	Super::BeginPlay();
	ShouldSpawnBoss = false;
	
	DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!DonutGameMode)
		return;
	ArrayEnemies = DonutGameMode->EnemiesArray;
	EnvironmentObjectsArraySmallxSmall = DonutGameMode->EnvironmentObjectsArraySmallxSmall;
	EnvironmentObjectsArrayBigxSmall = DonutGameMode->EnvironmentObjectsArrayBigxSmall;
	EnvironmentObjectsArraySmallxBig = DonutGameMode->EnvironmentObjectsArraySmallxBig;
	EnvironmentObjectsArrayBigxBig = DonutGameMode->EnvironmentObjectsArrayBigxBig;
	BossClass = DonutGameMode->BossClass;
	DoorClass = DonutGameMode->DoorClass;
}

// Called every frame
void ATC_DungeonRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*
* Set all the elements of the room
*/
void ATC_DungeonRoom::InitializeRoom(FVector RoomPosition, FVector RoomSize,
	TMap<FString, UInstancedStaticMeshComponent*> InstancesMeshMap, float TileSize, bool RoomBoss,
	bool RoomFinalDoor)
{
	// Save the data in the room
	DungeonRoomPosition = RoomPosition;
	DungeonRoomSize = RoomSize;
	MeshInstancesMap = InstancesMeshMap;
	DungeonTileSize = TileSize;
	ShouldSpawnBoss = RoomBoss;

	CreateFloor();
	CreateWalls();
	SetEnvironmentObjects();

	if (RoomFinalDoor)
		SetFinalDoor();
}

/*
* Create the room
*/
void ATC_DungeonRoom::CreateFloor()
{
	int SizeX = DungeonRoomSize.X;
	int SizeY = DungeonRoomSize.Y;

	if (MeshInstancesMap.Num() == 0)
		return;
	// Get the component to instance floors
	UInstancedStaticMeshComponent* InstancedFloorComponent = *MeshInstancesMap.Find("Floor");
	UInstancedStaticMeshComponent* InstancedFloor2Component = *MeshInstancesMap.Find("Corridor");
	if (!InstancedFloorComponent || !InstancedFloor2Component)
		return;

	int RandomFloor;
	for (int x = 0; x < SizeX; x++)
	{
		for (int y = 0; y < SizeY; y++)
		{
			FVector Tile = FVector(x * DungeonTileSize, y * DungeonTileSize, 0) + (DungeonRoomPosition * DungeonTileSize);

			RandomFloor = FMath::RandRange(0, 1);
			/*if(RandomFloor == 0)*/
				InstancedFloorComponent->AddInstance(FTransform(Tile));
			/*else
				InstancedFloor2Component->AddInstance(FTransform(Tile));*/
		}
	}
}

void ATC_DungeonRoom::SavePositionForCorridors(FVector RoomCorridorPosition)
{
	CorridorPositions.Add(RoomCorridorPosition);
}

// Create the walls of the room
void ATC_DungeonRoom::CreateWalls()
{
	int SizeX = DungeonRoomSize.X;
	int SizeY = DungeonRoomSize.Y;

	if(MeshInstancesMap.Num() == 0)
		return;
	UInstancedStaticMeshComponent* InstancedWallComponent = *MeshInstancesMap.Find("Wall");
	if (!InstancedWallComponent)
		return;

	// create the walls
	for (int x = 0; x <= SizeX; x++)
	{
		for (int y = 0; y <= SizeY; y++)
		{
			if (CheckIfCorridorInPosition(x, y))
				continue;

			if (x == 0 || x == SizeX || ((x > 0 && x < SizeX) && (y == 0 || y == SizeY)))
			{
				FVector Tile = FVector(x * DungeonTileSize, y * DungeonTileSize, 35.0f) + (DungeonRoomPosition * DungeonTileSize);


				FRotator WallRotation = FRotator::ZeroRotator;

				if (x == 0)
				{
					WallRotation = FRotator(0.f, -90.f, 0.f);
				}
				if (x == SizeX)
				{
					WallRotation = FRotator(0.f, -90.f, 0.f);
				}
				if ((y == SizeY || y == 0) && x != 0)
				{
					WallRotation = FRotator(0.f, 0.f, 0.f);
				}
				if (y == 0 && x == SizeX)
				{
					WallRotation = FRotator(0.f, -90.f, 0.f);
				}
				// do not put wall in this position
				if (y == SizeY && x == 0 || y == SizeY && x == SizeX)
				{
					continue;
				}
				InstancedWallComponent->AddInstance(FTransform(WallRotation, Tile));
			}

		}
	}

	// Extra walls
	FVector Tile = FVector((SizeX - 1) * DungeonTileSize, 0, 35.0f) + (DungeonRoomPosition * DungeonTileSize);
	InstancedWallComponent->AddInstance(FTransform(Tile));
	Tile = FVector(0 * DungeonTileSize, 0, 35.0f) + (DungeonRoomPosition * DungeonTileSize);
	InstancedWallComponent->AddInstance(FTransform(Tile));
	Tile = FVector(0, SizeY * DungeonTileSize, 35.0f) + (DungeonRoomPosition * DungeonTileSize);
	InstancedWallComponent->AddInstance(FTransform(Tile));
}

/*
* Check if there is a corridor in this position to not put a wall
*/
bool ATC_DungeonRoom::CheckIfCorridorInPosition(int XPos, int YPos)
{
	FVector PosToCheck = FVector(XPos, YPos, 0) + DungeonRoomPosition;
	if (CorridorPositions.Num() <= 0)
		return false;

	if (CorridorPositions.Find(PosToCheck) != INDEX_NONE)
	{
		CorridorPositions.Remove(PosToCheck);
		return true;
	}
	else
		return false;
}

void ATC_DungeonRoom::SetEnvironmentObjects()
{
	// Create the environment objects and enemies
	if (ArrayEnemies.Num() <= 0)
		return;

	float RoomMiddleX = (DungeonRoomSize.X - 1) * 0.5f * DungeonTileSize + DungeonRoomPosition.X * DungeonTileSize;
	float RoomMiddleY = (DungeonRoomSize.Y - 1) * 0.5f * DungeonTileSize + DungeonRoomPosition.Y * DungeonTileSize;
	FVector MiddleRoom = FVector(RoomMiddleX, RoomMiddleY, 50.f);

	TArray<TSubclassOf<ATC_EnvironmentObjects>> ObjectArrayToUse;
	if (DungeonRoomSize.X < 10 && DungeonRoomSize.Y < 10)
	{
		ObjectArrayToUse = EnvironmentObjectsArraySmallxSmall;
	}
	else if (DungeonRoomSize.X > 10 && DungeonRoomSize.Y < 10)
	{
		ObjectArrayToUse = EnvironmentObjectsArrayBigxSmall;
	}
	else if (DungeonRoomSize.X < 10 && DungeonRoomSize.Y > 10)
	{
		ObjectArrayToUse = EnvironmentObjectsArraySmallxBig;
	}
	else if (DungeonRoomSize.X > 10 && DungeonRoomSize.Y > 10)
	{
		ObjectArrayToUse = EnvironmentObjectsArrayBigxBig;
	}

	if (ObjectArrayToUse.Num() == 0)
		return;

	ATC_EnvironmentObjects* Object = GetWorld()->SpawnActor<ATC_EnvironmentObjects>(
		ObjectArrayToUse[0], MiddleRoom, FRotator::ZeroRotator);
	Object->Initialize(ShouldSpawnBoss, BossClass, DungeonTileSize, ArrayEnemies);
}

/*
* Set final door for the dungeon
*/
void ATC_DungeonRoom::SetFinalDoor()
{
	// Check where there is no corridor in the room
	int SizeX = DungeonRoomSize.X;
	int SizeY = DungeonRoomSize.Y;
	bool DoorSet = false;
	int NumIterations = 0;
	int MaxIterations = 50;

	int RandomDoorPositionYAxis;
	while (!DoorSet)
	{
		// generate another random position
		RandomDoorPositionYAxis = FMath::RandRange(0, SizeY - 1);

		if (!CheckIfCorridorInPosition(SizeX, RandomDoorPositionYAxis))
		{
			FVector DungeonDoorPosition = FVector(DungeonRoomPosition.X - 0.9f + SizeX, DungeonRoomPosition.Y + RandomDoorPositionYAxis, 0.f);
			ATC_Door* LastDoor = GetWorld()->SpawnActor<ATC_Door>(DoorClass, DungeonDoorPosition * DungeonTileSize, FRotator(0.f, 90.f, 0.f));
			LastDoor->SetActorScale3D(FVector(1.f, 1.f, 1.f));
			DoorSet = true;
		}
		if (NumIterations >= MaxIterations)
			return;

		NumIterations++;
	}
}