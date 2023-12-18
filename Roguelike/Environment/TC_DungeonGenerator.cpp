// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_DungeonGenerator.h"
#include "Math/UnrealMathUtility.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "TC_DungeonRoom.h"
#include "TC_DungeonCorridor.h"

// Sets default values
ATC_DungeonGenerator::ATC_DungeonGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FloorMeshObject(TEXT("/Game/Assets/Environment/SM_DungeonFloor"));
	if (FloorMeshObject.Object != NULL)
	{
		FloorMesh = FloorMeshObject.Object;
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WallMeshObject(TEXT("/Game/Assets/Architecture/Walls/SM_DungeonWall"));
	if (WallMeshObject.Object != NULL)
	{
		WallMesh = WallMeshObject.Object;
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CorridorMeshObject(TEXT("/Game/Assets/Environment/SM_DungeonCorridor"));
	if (CorridorMeshObject.Object != NULL)
	{
		CorridorMesh = CorridorMeshObject.Object;
	}

	InstancedStaticRoomMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticRoomMeshComponent"));
	InstancedStaticWallMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticWallMeshComponent"));
	InstancedStaticCorridorMeshComponent = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedStaticCorridorMeshComponent"));
}

// Called when the game starts or when spawned
void ATC_DungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
	
	DungeonSize = 0;
	MaxRoomSize = 0;
	MinRoomSize = 0;
	MaxNumberOfRooms = 0;
	NumberOfRetryPlacingRoom = 0;

	InitializeStaticMeshComponents();
}

/*
* Save all the instances responsible of instancing meshes
*/
void ATC_DungeonGenerator::InitializeStaticMeshComponents()
{
	InstancesMeshMap.Add("Floor", InstancedStaticRoomMeshComponent);
	InstancesMeshMap.Add("Corridor", InstancedStaticCorridorMeshComponent);
	InstancesMeshMap.Add("Wall", InstancedStaticWallMeshComponent);

	if (!FloorMesh || !InstancedStaticRoomMeshComponent)
		return;

	InstancedStaticRoomMeshComponent->SetStaticMesh(FloorMesh);
	InstancedStaticRoomMeshComponent->SetMobility(EComponentMobility::Static);
	InstancedStaticRoomMeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);

	if (!WallMesh || !InstancedStaticWallMeshComponent)
		return;

	InstancedStaticWallMeshComponent->SetStaticMesh(WallMesh);

	if (!CorridorMesh || !InstancedStaticWallMeshComponent)
		return;

	InstancedStaticCorridorMeshComponent->SetStaticMesh(CorridorMesh);
}

// Called every frame
void ATC_DungeonGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATC_DungeonGenerator::SetDungeonGeneratorVariables(int GDungeonSize,
	int GMaxRoomSize, int GMinRoomSize, int GMaxNumberOfRooms, int GNumberOfRetryPlacingRoom, float GTileSize)
{
	TileSize = GTileSize;
	DungeonSize = GDungeonSize;
	MaxRoomSize = GMaxRoomSize;
	MinRoomSize = GMinRoomSize;
	MaxNumberOfRooms = GMaxNumberOfRooms;
	NumberOfRetryPlacingRoom = GNumberOfRetryPlacingRoom;
}

FVector ATC_DungeonGenerator::GenerateAllDungeonRooms(int GDungeonSize, int GMaxRoomSize, 
	int GMinRoomSize, int GMaxNumberOfRooms, int GNumberOfRetryPlacingRoom, float GTileSize)
{
	SetDungeonGeneratorVariables(GDungeonSize, GMaxRoomSize, GMinRoomSize, GMaxNumberOfRooms,
		GNumberOfRetryPlacingRoom, GTileSize);
	
	// while the number of rooms is not yet the maximum and tries are below the max number of tries
	// continue trying to generate rooms
	while (RoomsPositionArray.Num() < MaxNumberOfRooms && CountTriesPlacing < NumberOfRetryPlacingRoom)
	{
		FVector NewRoomPosition;
		FVector NewRoomSize;

		if (TryPlacingARoom(NewRoomPosition, NewRoomSize))
		{
			// if succeeded placing the room, add it to the rooms arrays
			RoomsPositionArray.Add(NewRoomPosition);
			RoomSizesArray.Add(NewRoomSize);
			CountTriesPlacing = 0;
		}
		else
		{
			CountTriesPlacing++;
		}
	}

	// SPAWN ROOMS
	for (int i = 0; i < RoomsPositionArray.Num(); i++)
	{
		// Create an object type dungeon room
		ATC_DungeonRoom* DungeonRoom = GetWorld()->SpawnActor<ATC_DungeonRoom>(ATC_DungeonRoom::StaticClass(),
			RoomsPositionArray[i], FRotator::ZeroRotator);

		DungeonRoomsArray.Add(DungeonRoom);
	}

	// CORRIDORS
	GenerateCorridors();


	// Get the position of the player at start
	FVector PlayerFirstPosition = RoomsPositionArray[0];

	// Get the position for the boss and the final door
	FVector BossRoomPosition = GetFurthestRoomToPosition(PlayerFirstPosition, PlayerFirstPosition);
	FVector FinalDoorRoomPosition = GetFurthestRoomToPosition(BossRoomPosition, PlayerFirstPosition);

	// Intialize the rooms
	for (int i = 0; i < RoomsPositionArray.Num(); i++)
	{
		bool RoomBoss = false;
		bool RoomFinalDoor = false;

		// If the room is the boss room
		if (BossRoomPosition == RoomsPositionArray[i])
			RoomBoss = true;
		else if (FinalDoorRoomPosition == RoomsPositionArray[i])
			RoomFinalDoor = true;

			DungeonRoomsArray[i]->InitializeRoom(RoomsPositionArray[i], RoomSizesArray[i],
			InstancesMeshMap, TileSize, RoomBoss, RoomFinalDoor);
	}


	// Empty arrays
	CompletedRoomsArray.Empty();
	RestRoomsArray.Empty();
	RoomsPositionArray.Empty();
	RoomSizesArray.Empty();

	return PlayerFirstPosition * TileSize;
}

#pragma region ROOM
/*
* Generate a random room size and position
*/
void ATC_DungeonGenerator::GenerateRoom(FVector& RoomPosition, FVector& RoomSize)
{
	// Get a randomsize between the possible room size range
	int RandomSizeX = FMath::RandRange(MinRoomSize, MaxRoomSize);
	int RandomSizeY = FMath::RandRange(MinRoomSize, MaxRoomSize);

	RoomSize = FVector(RandomSizeX, RandomSizeY, 0);
	
	// To avoid that the room extends the DUNGEON SIZE, bound position between 0 and previously defined size
	int RoomSizeX = FMath::TruncateToHalfIfClose(RoomSize.X);
	int RoomSizeY = FMath::TruncateToHalfIfClose(RoomSize.Y);

	// Random position for the room in the dungeon
	int RandomPositionX = FMath::RandRange(0, DungeonSize - RoomSizeX);
	int RandomPositionY = FMath::RandRange(0, DungeonSize - RoomSizeY);
	RoomPosition = FVector(RandomPositionX, RandomPositionY, 0);
	
}

/*
* Try to place the generated room in the dungeon without overlapping another existing room
*/
bool ATC_DungeonGenerator::TryPlacingARoom(FVector& RoomPosition, FVector& RoomSize)
{
	bool RoomOverlapping = false;
	// Generate new random room
	GenerateRoom(RoomPosition, RoomSize);

	// Check if overlapping rest of the rooms
	for (int i = 0; i < RoomsPositionArray.Num(); i++)
	{
		RoomOverlapping = IfRoomsOverlapping(RoomsPositionArray[i], RoomSizesArray[i], RoomPosition, RoomSize);
		if (RoomOverlapping)
			return false;
	}

	// if overlapping room, return true, because we succeeded placing the new room
	return true;
}

bool ATC_DungeonGenerator::IfRoomsOverlapping(FVector ExistingRoomPosition, FVector ExistingRoomSize,
	FVector NewRoomPosition, FVector NewRoomSize)
{
	//NEW ROOM LEFT
	if (NewRoomPosition.X < ExistingRoomPosition.X)
	{
		if (NewRoomPosition.Y < ExistingRoomPosition.Y)
		{
			//NEW ROOM BELOW
			if (NewRoomPosition.X + NewRoomSize.X >= ExistingRoomPosition.X &&
				NewRoomPosition.Y + NewRoomSize.Y >= ExistingRoomPosition.Y)
			{
				return true;
			}
		}
		else
		{
			//NEW ROOM ABOVE
			if (ExistingRoomPosition.Y + ExistingRoomSize.Y >= NewRoomPosition.Y &&
				ExistingRoomPosition.X <= NewRoomPosition.X + NewRoomSize.X)
			{
				return true;
			}
		}
	}
	else
	{
		//NEW ROOM RIGHT
		if (NewRoomPosition.Y < ExistingRoomPosition.Y)
		{
			//NEW ROOM BELOW
			if (NewRoomPosition.Y + NewRoomSize.Y >= ExistingRoomPosition.Y &&
				NewRoomPosition.X <= ExistingRoomPosition.X + ExistingRoomSize.X)
			{
				return true;
			}
		}
		else
		{
			//NEW ROOM ABOVE
			if (ExistingRoomPosition.X + ExistingRoomSize.X >= NewRoomPosition.X &&
				ExistingRoomPosition.Y + ExistingRoomSize.Y >= NewRoomPosition.Y)
			{
				return true;
			}
		}
	}

	return false;
}

#pragma endregion ROOM

#pragma region CORRIDOR

/* CORRIDOR FUNCTIONS */

void ATC_DungeonGenerator::GenerateCorridors()
{
	TArray<FVector> CorridorRoomRoomStart;
	TArray<FVector> CorridorRoomRoomEnd;
	TArray<FVector> CorridorRoomCornerStart;
	TArray<FVector> CorridorRoomCornerEnd;
	CreatePosAndIndex();
	GenerateSpanningTree();

	

	// create corridor for each edge between rooms
	for (FVector Edge : EdgesArray)
	{
		// Get the indexes of both rooms saved in Edge
		int CompletedRoomIndex = Edge.X;
		int RestRoomIndex = Edge.Y;
		// these indexes have the position of the rooms in the main RoomsPositionArray
		CreateCorridor(CompletedRoomIndex, RestRoomIndex, CorridorRoomRoomStart, 
			CorridorRoomRoomEnd, CorridorRoomCornerStart, CorridorRoomCornerEnd);

	}

	// SPAWN CORRIDORS
	ATC_DungeonCorridor* NewDungeonCorridor;
	if (CorridorRoomCornerStart.Num())
	{
		for (int i = 0; i < CorridorRoomCornerStart.Num(); i++)
		{
			NewDungeonCorridor = GetWorld()->SpawnActor<ATC_DungeonCorridor>(ATC_DungeonCorridor::StaticClass(),
				CorridorRoomCornerStart[i], FRotator::ZeroRotator);
			NewDungeonCorridor->SpawnDungeonCorridor(CorridorRoomCornerStart[i], CorridorRoomCornerEnd[i],
				InstancesMeshMap, TileSize);
		}
	}
	
	if (!CorridorRoomRoomStart.Num())
		return;
	for (int i = 0; i < CorridorRoomRoomStart.Num(); i++)
	{
		NewDungeonCorridor = GetWorld()->SpawnActor<ATC_DungeonCorridor>(ATC_DungeonCorridor::StaticClass(),
			CorridorRoomRoomStart[i], FRotator::ZeroRotator);
		NewDungeonCorridor->SpawnDungeonCorridor(CorridorRoomRoomStart[i], CorridorRoomRoomEnd[i],
			InstancesMeshMap, TileSize);
	}
}

/*
* Check where room overlapps
		// 0 OVERLAPPING IN X AXIS
		// 1 OVERLAPPING IN Y AXIS
		// 2 NO OVERLAPPING
*/
int ATC_DungeonGenerator::CheckRoomsOverlappingAxis(float CompletedRoomXLeft, float RestRoomXLeft,
	float CompletedRoomXRight, float RestRoomXRight, float CompletedRoomYDown, float RestRoomYDown,
	float CompletedRoomYTop, float RestRoomYTop)
{
	if (FMath::Max(CompletedRoomXLeft + 1, RestRoomXLeft + 1) <=
		FMath::Min(CompletedRoomXRight - 1, RestRoomXRight - 1))
	{
		return 0;
	}
	else if (FMath::Max(CompletedRoomYDown + 1, RestRoomYDown + 1) <=
		FMath::Min(CompletedRoomYTop - 1, RestRoomYTop - 1))
	{
		return 1;
	}
	else
	{
		return 2;
	}
		
}

/*
* Check the position of the complete and rest room and connect them
*/
void ATC_DungeonGenerator::CreateCorridor(int CompletedRoomIndex, int RestRoomIndex, TArray<FVector>& RoomRoomStart,
	TArray<FVector>& RoomRoomEnd, TArray<FVector>& RoomCorridorStart, TArray<FVector>& RoomCorridorEnd)
{
	// these indexes have the position of the rooms in the main arrays
	FVector CompletedRoomPosition = RoomsPositionArray[CompletedRoomIndex];
	FVector CompletedRoomSize = RoomSizesArray[CompletedRoomIndex];
	FVector RestRoomPosition = RoomsPositionArray[RestRoomIndex];
	FVector RestRoomSize = RoomSizesArray[RestRoomIndex];

	// ROOMS OBJECTS
	ATC_DungeonRoom* DungeonRoomStart = DungeonRoomsArray[CompletedRoomIndex];
	ATC_DungeonRoom* DungeonRoomEnd = DungeonRoomsArray[RestRoomIndex];

	// Get the corners in the x axis for both rooms (see if overlapping in x axis)
	float CompletedRoomXLeft = CompletedRoomPosition.X;
	float CompletedRoomXRight = CompletedRoomPosition.X + CompletedRoomSize.X;
	float RestRoomXLeft = RestRoomPosition.X;
	float RestRoomXRight = RestRoomPosition.X + RestRoomSize.X;
	// Get the corners in the y axis for both rooms
	float CompletedRoomYDown = CompletedRoomPosition.Y;
	float CompletedRoomYTop = CompletedRoomPosition.Y + CompletedRoomSize.Y;
	float RestRoomYDown = RestRoomPosition.Y;
	float RestRoomYTop = RestRoomPosition.Y + RestRoomSize.Y;

	FVector CorridorStart;
	FVector CorridorEnd;

	// check if both rooms are overlapping in X axis
	if (CheckRoomsOverlappingAxis(CompletedRoomXLeft, RestRoomXLeft, CompletedRoomXRight, RestRoomXRight, 
		CompletedRoomYDown, RestRoomYDown, CompletedRoomYTop, RestRoomYTop) == 0)
	{
		// OVERLAPPING X AXIS
		int RandomX = FMath::RandRange(FMath::Max(CompletedRoomXLeft + 1, RestRoomXLeft + 1), FMath::Min(CompletedRoomXRight - 1, RestRoomXRight - 1));
		
		if (CompletedRoomYDown <= RestRoomYDown)
		{
			// IF THE COMPLETED ROOM IS BELOW THE RESTROOM
			// Vector of completed room (Y Axis TOP)
			CorridorStart = FVector(RandomX, CompletedRoomYTop, 0.f);
			// Vector of rest room (Y Axis BOTTOM)
			CorridorEnd = FVector(RandomX, RestRoomYDown, 0.f);

		}
		else
		{
			// COMPLETED ROOM ABOVE
			// Vector of completed room (Y Axis BOTTOM)
			CorridorStart = FVector(RandomX, CompletedRoomYDown, 0.f);
			// Vector of rest room (Y Axis TOP)
			CorridorEnd = FVector(RandomX, RestRoomYTop, 0.f);
		}

		RoomRoomStart.Add(CorridorStart);
		RoomRoomEnd.Add(CorridorEnd);

	}
	else if(CheckRoomsOverlappingAxis(CompletedRoomXLeft, RestRoomXLeft, CompletedRoomXRight, RestRoomXRight,
		CompletedRoomYDown, RestRoomYDown, CompletedRoomYTop, RestRoomYTop) == 1)
	{
		// OVERLAPPING Y AXIS
		int RandomY = FMath::RandRange(FMath::Max(CompletedRoomYDown + 1, RestRoomYDown + 1), FMath::Min(CompletedRoomYTop - 1, RestRoomYTop - 1));
		
		if (CompletedRoomXLeft < RestRoomXLeft)
		{
			// IF THE COMPLETED ROOM IS ON THE LEFT
			// Vector of completed room (X Axis RIGHT)
			CorridorStart = FVector(CompletedRoomXRight, RandomY, 0.f);
			// Vector of rest room (X Axis LEFT)
			CorridorEnd = FVector(RestRoomXLeft, RandomY, 0.f);
		}
		else
		{
			// IF THE COMPLETED ROOM IS ON THE RIGHT
			// Vector of completed room (X Axis LEFT)
			CorridorStart = FVector(CompletedRoomXLeft, RandomY, 0.f);
			// Vector of rest room (X Axis RIGHT)
			CorridorEnd = FVector(RestRoomXRight, RandomY, 0.f);
		}

		// add the vectors to the arrays
		RoomRoomStart.Add(CorridorStart);
		RoomRoomEnd.Add(CorridorEnd);
	}
	else
	{
		// NO OVERLAPPING IN ANY AXIS
		NoOverlappingAxisCorridor(CompletedRoomXLeft, CompletedRoomXRight, RestRoomXLeft, RestRoomXRight,
			CompletedRoomYDown, CompletedRoomYTop, RestRoomYDown, RestRoomYTop, RoomCorridorStart, RoomCorridorEnd,
			DungeonRoomStart, DungeonRoomEnd);
	}


	// Save in the dungeon rooms where not to put a wall, because there is a corridor
	// DUNGEON ROOM TO ROOM
	if (DungeonRoomStart)
	{
		DungeonRoomStart->SavePositionForCorridors(CorridorStart);
	}
	if (DungeonRoomEnd)
	{
		DungeonRoomEnd->SavePositionForCorridors(CorridorEnd);
	}
}

/*
* Create corridor when no overlapping axis
*/
void ATC_DungeonGenerator::NoOverlappingAxisCorridor(float CompletedRoomXLeft, float CompletedRoomXRight,
	float RestRoomXLeft, float RestRoomXRight, float CompletedRoomYDown, float CompletedRoomYTop,
	float RestRoomYDown, float RestRoomYTop, TArray<FVector>& RoomCorridorStart, TArray<FVector>& RoomCorridorEnd,
	ATC_DungeonRoom* DungeonRoomCompleted, ATC_DungeonRoom* DungeonRoomRest)
{
	if (!DungeonRoomCompleted || !DungeonRoomRest)
		return;

	float RandomWay = FMath::RandRange(0, 1);
	// if more than 0.5 take option B
	float Room1YDown;
	float Room1YTop;
	float Room1XLeft;
	float Room1XRight;
	float Room2YDown;
	float Room2YTop;
	float Room2XLeft;
	float Room2XRight;
	ATC_DungeonRoom* DungeonRoom1;
	ATC_DungeonRoom* DungeonRoom2;

	if (RandomWay > 0.5f)
	{
		// the room 1 is gonna be the completed room
		Room1YDown = CompletedRoomYDown;
		Room1YTop = CompletedRoomYTop;
		Room1XLeft = CompletedRoomXLeft;
		Room1XRight = CompletedRoomXRight;
		DungeonRoom1 = DungeonRoomCompleted;

		Room2YDown = RestRoomYDown;
		Room2YTop = RestRoomYTop;
		Room2XLeft = RestRoomXLeft;
		Room2XRight = RestRoomXRight;
		DungeonRoom2 = DungeonRoomRest;
	}
	else
	{
		//OPTION A
		// the room 1 is gonna be the rest room
		Room1YDown = RestRoomYDown;
		Room1YTop = RestRoomYTop;
		Room1XLeft = RestRoomXLeft;
		Room1XRight = RestRoomXRight;
		DungeonRoom1 = DungeonRoomRest;

		Room2YDown = CompletedRoomYDown;
		Room2YTop = CompletedRoomYTop;
		Room2XLeft = CompletedRoomXLeft;
		Room2XRight = CompletedRoomXRight;
		DungeonRoom2 = DungeonRoomCompleted;
	}
	// Get random points in each room for the corridors
	int Room1YRandom = FMath::RandRange(Room1YDown + 1, Room1YTop - 1);
	int Room2XRandom = FMath::RandRange(Room2XLeft + 1, Room2XRight - 1);

	///////////////// CHECK IF ROOM 1 ABOVE OR BELOW ////////////////
	// 
	FVector StartCorridorRoom1;
	FVector StartCorridorRoom2;

	// Corner --> 1 LB, 2 LT, 3 RT, 4 RB
	float NumberCorner = 0.f;
	// IF ROOM 1 IS BELOW
	if (Room1YDown <= Room2YDown)
	{
		// Start for the corridor in room 2
		StartCorridorRoom2 = FVector(Room2XRandom, Room2YDown, 0);
		NumberCorner = 1; // BOTTOM
	}
	else
	{
		//// IF ROOM 1 IS ABOVE
		// Start for the corridor in room 2
		StartCorridorRoom2 = FVector(Room2XRandom, Room2YTop, 0);
		NumberCorner = 2; // TOP

	}

	///////////////// CHECK IF ROOM 1 LEFT OR RIGHT ////////////////

	if (Room1XLeft <= Room2XLeft)
	{
		// IF ROOM 1 IS LEFT (the vector is on the right part)
		// Start for the corridor in room 1
		StartCorridorRoom1 = FVector(Room1XRight, Room1YRandom, 0);

		if (NumberCorner == 1)
			NumberCorner = 1; // BOTTOM LEFT
		else if (NumberCorner == 2)
			NumberCorner = 4; // BOTTOM RIGHT
	}
	else
	{
		// IF ROOM 1 IS RIGHT (the vector is on the left part)
		// Start for the corridor in room 1
		StartCorridorRoom1 = FVector(Room1XLeft, Room1YRandom, 0);

		if (NumberCorner == 1)
			NumberCorner = 2; // TOP LEFT
		else if (NumberCorner == 2)
			NumberCorner = 3; // TOP RIGHT
	}

	RoomCorridorStart.Add(StartCorridorRoom1);
	RoomCorridorStart.Add(StartCorridorRoom2);
	DungeonRoom1->SavePositionForCorridors(StartCorridorRoom1);
	DungeonRoom2->SavePositionForCorridors(StartCorridorRoom2);

	// BOTH ROOMS CORRIDOR END IN THE SAME POSITION
	// set a 1 in Z axis to know it is the CORNER
	FVector EndCorridor = FVector(Room2XRandom, Room1YRandom, NumberCorner);
	// End for the corridor in room 1 (Y of first room and X of second)
	RoomCorridorEnd.Add(EndCorridor);
	// End for the corridor in room 2 (Y of first room and X of second)
	RoomCorridorEnd.Add(EndCorridor);
}

/*
* Get the middle of all the rooms and save it in the RESTROOMS array
*/
void ATC_DungeonGenerator::CreatePosAndIndex()
{
	// save the CENTER position of all rooms
	for (int i = 0; i < RoomsPositionArray.Num(); i++)
	{
		// save in the array the mid position of the room in the dungeon, and index as Z axis
		float MidPosX = (RoomSizesArray[i].X * 0.5) + RoomsPositionArray[i].X;
		float MidPosY = (RoomSizesArray[i].Y * 0.5) + RoomsPositionArray[i].Y;
		RestRoomsArray.Add(FVector(MidPosX, MidPosY, i));
	}
}

/*
* Generate connections between the rroms
*/
void ATC_DungeonGenerator::GenerateSpanningTree()
{
	// Add the first room to completed rooms and remove it from rest array
	FVector FirstRoom = RestRoomsArray[0];
	CompletedRoomsArray.Add(FirstRoom);
	RestRoomsArray.Remove(FirstRoom);

	

	// iterate until all the rooms are connected with corridors
	while (RestRoomsArray.Num() > 0)
	{
		int IndexDone;
		int IndexSearch;
		FindMinimalDistanceForRooms(IndexDone, IndexSearch);
		
		// Create a link between them
		int CompletedRoomIndex = CompletedRoomsArray[IndexDone].Z;
		int RestRoomIndex = RestRoomsArray[IndexSearch].Z;
		EdgesArray.Add(FVector(CompletedRoomIndex, RestRoomIndex, 0));

		// add the room to completed part
		FVector ConnectedRoom = RestRoomsArray[IndexSearch];
		CompletedRoomsArray.Add(ConnectedRoom);
		RestRoomsArray.Remove(ConnectedRoom);
	}
}

/*
* Determine which "Completed" room is the closest to which "Rest" room
*/
void ATC_DungeonGenerator::FindMinimalDistanceForRooms(int& IndexDone, int& IndexSearch)
{
	float MinDist = 0;
	
	// compare each doneroom with each rest room to connect the closest rooms together
	for (int i = 0; i < CompletedRoomsArray.Num(); i++)
	{
		// Get the corners in the x and y axis for the completed room
		float CompletedRoomXLeft = RoomsPositionArray[CompletedRoomsArray[i].Z].X;
		float CompletedRoomXRight = RoomsPositionArray[CompletedRoomsArray[i].Z].X + RoomSizesArray[CompletedRoomsArray[i].Z].X;
		float CompletedRoomYDown = RoomsPositionArray[CompletedRoomsArray[i].Z].Y;
		float CompletedRoomYTop = RoomsPositionArray[CompletedRoomsArray[i].Z].Y + RoomSizesArray[CompletedRoomsArray[i].Z].Y;
		
		for (int j = 0; j < RestRoomsArray.Num(); j++)
		{
			
			// get position in another vector, because DoneRoom has the index of the room in Z axis
			FVector DoneRoomMiddlePosition = FVector(CompletedRoomsArray[i].X, CompletedRoomsArray[i].Y, 0);
			FVector RestRoomMiddlePosition = FVector(RestRoomsArray[j].X, RestRoomsArray[j].Y, 0);
			// distance between both rooms (MIDDLE)
			float Distance = FVector::Dist(DoneRoomMiddlePosition, RestRoomMiddlePosition);

			// if the distance between the two rooms is less than a previously rooms distance
			if (MinDist == 0 || MinDist > Distance)
			{
				// Get the corners in the x and y axis for the rest room
				float RestRoomXLeft = RoomsPositionArray[RestRoomsArray[j].Z].X;
				float RestRoomXRight = RoomsPositionArray[RestRoomsArray[j].Z].X + RoomSizesArray[RestRoomsArray[j].Z].X;
				float RestRoomYDown = RoomsPositionArray[RestRoomsArray[j].Z].Y;
				float RestRoomYTop = RoomsPositionArray[RestRoomsArray[j].Z].Y + RoomSizesArray[RestRoomsArray[j].Z].Y;

				// check if the rooms overlapp in any axis - if 2 no overlapping in X or Y axis
				if (CheckRoomsOverlappingAxis(CompletedRoomXLeft, RestRoomXLeft, CompletedRoomXRight, RestRoomXRight,
					CompletedRoomYDown, RestRoomYDown, CompletedRoomYTop, RestRoomYTop) == 2)
				{
					// NO OVERLAPPING
					if (CheckIfAnyRoomInThisPosition(CompletedRoomXLeft, RestRoomXLeft, CompletedRoomXRight, RestRoomXRight,
						CompletedRoomYDown, RestRoomYDown, CompletedRoomYTop, RestRoomYTop))
					{
						// there is already a room that can overlapp with the corridor
						// NO CONNECTION BETWEEN THIS ROOM - CONTINUE
						continue;
					}
					
				}

				MinDist = Distance;
				// save the index of these rooms as the closest rooms
				IndexDone = i;
				IndexSearch = j;
			}
		}
	}
}

/*
* check if there is any room that can overlapp the corridor between these two rooms
*/
bool ATC_DungeonGenerator::CheckIfAnyRoomInThisPosition(float CompletedRoomXLeft, float RestRoomXLeft,
	float CompletedRoomXRight, float RestRoomXRight, float CompletedRoomYDown, float RestRoomYDown,
	float CompletedRoomYTop, float RestRoomYTop)
{
	// check all the rooms in the map
	for (int i = 0; i < RoomsPositionArray.Num(); i++)
	{
		TArray<FVector> RoomCorners;
		// GET CORNERS OF THE ROOM
		RoomCorners.Add(FVector(RoomsPositionArray[i].X, RoomsPositionArray[i].Y, 0.f));
		RoomCorners.Add(FVector(RoomsPositionArray[i].X, RoomsPositionArray[i].Y + RoomSizesArray[i].Y, 0.f));
		RoomCorners.Add(FVector(RoomsPositionArray[i].X + RoomSizesArray[i].X, RoomsPositionArray[i].Y, 0.f));
		RoomCorners.Add(FVector(RoomsPositionArray[i].X + RoomSizesArray[i].X, RoomsPositionArray[i].Y + RoomSizesArray[i].Y, 0.f));
		
		for (FVector Corner : RoomCorners)
		{
			// check if any corner of the room is placed where the completed and rest room could place the corridor
			if (Corner.X > RestRoomXLeft && Corner.X < RestRoomXRight &&
				Corner.Y > CompletedRoomYDown && Corner.Y < CompletedRoomYTop)
			{
				return true;
			}
			else if (Corner.X > CompletedRoomXLeft && Corner.X < CompletedRoomXRight &&
				Corner.Y > RestRoomYDown && Corner.Y < RestRoomYTop)
			{
				return true;
			}
		}
		
	}

	return false;
}

#pragma endregion CORRIDOR

FVector ATC_DungeonGenerator::GetFurthestRoomToPosition(FVector Position, FVector PositionToAvoid)
{
	float MaxDist = 0;
	int RoomIndex = 0;
	for (int i = 0; i < RoomsPositionArray.Num() - 1; i++)
	{
		// Cannot be this position
		if (RoomsPositionArray[i] == PositionToAvoid)
			continue;

		float DistToPlayer = FVector::Dist(RoomsPositionArray[i], Position);

		if (MaxDist <= 0 || MaxDist < DistToPlayer)
		{
			MaxDist = DistToPlayer;
			RoomIndex = i;
		}
	}
	return RoomsPositionArray[RoomIndex];
}