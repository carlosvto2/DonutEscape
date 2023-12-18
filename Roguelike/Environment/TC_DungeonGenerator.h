// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TC_DungeonGenerator.generated.h"

class UInstancedStaticMeshComponent;
class ATC_DungeonRoom;

UCLASS()
class ROGUELIKE_API ATC_DungeonGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATC_DungeonGenerator();
	virtual void Tick(float DeltaTime) override;
	FVector GenerateAllDungeonRooms(int GDungeonSize, int GMaxRoomSize,
		int GMinRoomSize, int GMaxNumberOfRooms, int GNumberOfRetryPlacingRoom, float GTileSize);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	TMap<FString, UInstancedStaticMeshComponent*> InstancesMeshMap;
	UInstancedStaticMeshComponent* InstancedStaticRoomMeshComponent;
	UInstancedStaticMeshComponent* InstancedStaticCorridorMeshComponent;
	UInstancedStaticMeshComponent* InstancedStaticWallMeshComponent;

	UStaticMesh* FloorMesh;
	UStaticMesh* WallMesh;
	UStaticMesh* CorridorMesh;

	void InitializeStaticMeshComponents();


	int DungeonSize;
	int MaxRoomSize;
	int MinRoomSize;
	int MaxNumberOfRooms;
	int NumberOfRetryPlacingRoom;
	int CountTriesPlacing;
	float TileSize;

	TArray<FVector> RoomsPositionArray;
	TArray<FVector> RoomSizesArray;
	TArray<ATC_DungeonRoom*> DungeonRoomsArray;

	void SetDungeonGeneratorVariables(int GDungeonSize, int GMaxRoomSize,
		int GMinRoomSize, int GMaxNumberOfRooms, int GNumberOfRetryPlacingRoom, float GTileSize);
	void GenerateRoom(FVector& RoomPosition, FVector& RoomSize);
	bool TryPlacingARoom(FVector& RoomPosition, FVector& RoomSize);
	bool IfRoomsOverlapping(FVector ExistingRoomPosition, FVector ExistingRoomSize, FVector NewRoomPosition, FVector NewRoomSize);

	// CORRIDOR
	TArray<FVector> CompletedRoomsArray;
	TArray<FVector> RestRoomsArray;
	TArray<FVector> EdgesArray;
	void SpawnCorridor(FVector Start, FVector End);
	void GenerateCorridors();
	int CheckRoomsOverlappingAxis(float CompletedRoomXLeft, float RestRoomXLeft,
		float CompletedRoomXRight, float RestRoomXRight, float CompletedRoomYDown, float RestRoomYDown,
		float CompletedRoomYTop, float RestRoomYTop);
	void CreateCorridor(int CompletedRoomIndex, int RestRoomIndex, TArray<FVector>& RoomRoomStart, TArray<FVector>& RoomRoomEnd, TArray<FVector>& RoomCorridorStart, TArray<FVector>& RoomCorridorEnd);
	void NoOverlappingAxisCorridor(float CompletedRoomXLeft, float CompletedRoomXRight,
		float RestRoomXLeft, float RestRoomXRight, float CompletedRoomYDown, float CompletedRoomYTop,
		float RestRoomYDown, float RestRoomYTop, TArray<FVector>& RoomCorridorStart, TArray<FVector>& RoomCorridorEnd,
		ATC_DungeonRoom* DungeonRoomCompleted, ATC_DungeonRoom* DungeonRoomRest);
	void CreatePosAndIndex();
	void GenerateSpanningTree();
	void FindMinimalDistanceForRooms(int& IndexDone, int& IndexSearch);
	bool CheckIfAnyRoomInThisPosition(float CompletedRoomXLeft, float RestRoomXLeft,
		float CompletedRoomXRight, float RestRoomXRight, float CompletedRoomYDown, float RestRoomYDown,
		float CompletedRoomYTop, float RestRoomYTop);

	FVector GetFurthestRoomToPosition(FVector Position, FVector PositionToAvoid);
};
