// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_DungeonCorridor.h"
#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
ATC_DungeonCorridor::ATC_DungeonCorridor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATC_DungeonCorridor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATC_DungeonCorridor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATC_DungeonCorridor::GetCorrectStartEndAxis(int &StartX, int &StartY, int &EndX, int &EndY,
	bool CheckCorner)
{
	// if the corridor is going top, bot, left or right
	if (CorridorStartPosition.X < CorridorEndPosition.X)
	{
		StartX = CorridorStartPosition.X;
		EndX = CorridorEndPosition.X;
		/*if (CorridorEndPosition.Z == 1.0f && CheckCorner)
		{
			StartX++;
		}*/
	}
	else
	{
		StartX = CorridorEndPosition.X;
		EndX = CorridorStartPosition.X;
	}
	if (CorridorStartPosition.Y < CorridorEndPosition.Y)
	{
		StartY = CorridorStartPosition.Y;
		EndY = CorridorEndPosition.Y;
	}
	else
	{
		StartY = CorridorEndPosition.Y;
		EndY = CorridorStartPosition.Y;
	}
}

void ATC_DungeonCorridor::SpawnDungeonCorridor(FVector Start, FVector End, 
	TMap<FString, UInstancedStaticMeshComponent*> InstancesMeshMap, int TileSize)
{
	// Save the data in the room
	CorridorStartPosition = Start;
	CorridorEndPosition = End;
	MeshInstancesMap = InstancesMeshMap;
	DungeonTileSize = TileSize;

	int StartX;
	int StartY;
	int EndX;
	int EndY;

	GetCorrectStartEndAxis(StartX, StartY, EndX, EndY, false);
	

	if (MeshInstancesMap.Num() == 0)
		return;
	// Get the component to instance floors
	UInstancedStaticMeshComponent* InstancedCorridorComponent = *MeshInstancesMap.Find("Corridor");
	if (!InstancedCorridorComponent)
		return;

	for (int x = StartX; x <= EndX; x++)
	{
		for (int y = StartY; y <= EndY; y++)
		{
			FVector Tile = FVector(x * DungeonTileSize, y * DungeonTileSize, -2.0f);

			InstancedCorridorComponent->AddInstance(FTransform(Tile));
		}
	}
	SpawnCorridorsWalls();
}


void ATC_DungeonCorridor::SpawnCorridorsWalls()
{
	int StartX;
	int StartY;
	int EndX;
	int EndY;

	GetCorrectStartEndAxis(StartX, StartY, EndX, EndY, true);

	if (MeshInstancesMap.Num() == 0)
		return;
	// Get the component to instance floors
	
	InstancedWallComponent = *MeshInstancesMap.Find("Wall");
	if (!InstancedWallComponent)
		return;

	
	if (StartX < EndX)
	{
		WallRotation1 = FRotator(0.f, 0.f, 0.f);
		WallRotation2 = FRotator(0.f, 180.f, 0.f);
	}
	else if (StartY < EndY)
	{
		WallRotation1 = FRotator(0.f, 90.f, 0.f);
		WallRotation2 = FRotator(0.f, -90.f, 0.f);
	}

	for (int x = StartX; x <= EndX; x++)
	{
		for (int y = StartY; y <= EndY; y++)
		{

			SetWallsDependingOnCorner(x, y);
		}
	}
}

void ATC_DungeonCorridor::SetWallsDependingOnCorner(int x, int y)
{
	int CornerValue = CorridorEndPosition.Z;
	FVector Tile = FVector(x * DungeonTileSize, y * DungeonTileSize, 35.f);

	// avoid to put a wall in the start position, because it gets inside the room - FOR ALL COMBINATIONS
	if (CorridorStartPosition.X > CorridorEndPosition.X && x == CorridorStartPosition.X
		|| CorridorStartPosition.Y > CorridorEndPosition.Y && y == CorridorStartPosition.Y)
		return;

	switch (CornerValue)
	{
		case 0:
			// avoid to put a wall in the end position, because it gets inside the room - FOR NON CORNERS CORRIDORS
			if (CorridorStartPosition.X < CorridorEndPosition.X && x == CorridorEndPosition.X
				|| CorridorStartPosition.Y < CorridorEndPosition.Y && y == CorridorEndPosition.Y)
				break;

			InstancedWallComponent->AddInstance(FTransform(WallRotation1, Tile));
			InstancedWallComponent->AddInstance(FTransform(WallRotation2, Tile));
			break;

		case 1: // CORNER BOTTOM LEFT
			// set the walls when we are in the corner
			CorridorCornerWallRotation(x, y, WallRotation1, WallRotation1);
			break;

		case 2: // CORNER BOTTOM RIGHT
			CorridorCornerWallRotation(x, y, WallRotation1, WallRotation2);
			break;

		case 3: // CORNER TOP RIGHT
			CorridorCornerWallRotation(x, y, WallRotation2, WallRotation2);
			break;

		case 4: // CORNER TOP LEFT
			CorridorCornerWallRotation(x, y, WallRotation2, WallRotation1);
			break;
	}
}

/*
* Set the wall values when there is a corner
*/
void ATC_DungeonCorridor::CorridorCornerWallRotation(int x, int y, FRotator WallRotationXAxis, 
	FRotator WallRotationYAxis)
{
	FVector Tile = FVector(x * DungeonTileSize, y * DungeonTileSize, 35.f);
	if (CorridorStartPosition.X != CorridorEndPosition.X)
	{
		// corridor moving in X axis
		if (x == CorridorEndPosition.X)
		{
			// We are in the corner (just set the outer Wall)
			InstancedWallComponent->AddInstance(FTransform(WallRotationXAxis, Tile));
			return;
		}
	}
	else
	{
		// corridor moving in Y axis
		if (y == CorridorEndPosition.Y)
		{
			// We are in the corner (just set the outer Wall)
			InstancedWallComponent->AddInstance(FTransform(WallRotationYAxis, Tile));
			return;
		}
	}
	InstancedWallComponent->AddInstance(FTransform(WallRotation1, Tile));
	InstancedWallComponent->AddInstance(FTransform(WallRotation2, Tile));
}