// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TC_DungeonRoom.generated.h"

class UInstancedStaticMeshComponent;
class ATC_GameMode;
class ATC_BaseCharacter;
class ATC_EnvironmentObjects;
class ATC_Boss;
class ATC_Door;

UCLASS()
class ROGUELIKE_API ATC_DungeonRoom : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATC_DungeonRoom();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SavePositionForCorridors(FVector RoomCorridorPosition);

	void InitializeRoom(FVector RoomPosition, FVector RoomSize,
		TMap<FString, UInstancedStaticMeshComponent*> InstancesMeshMap, float TileSize, bool RoomBoss, 
		bool RoomFinalDoor);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

private:
	FVector DungeonRoomPosition;
	FVector DungeonRoomSize;
	TArray<FVector> CorridorPositions;
	TArray<TSubclassOf<ATC_BaseCharacter>> ArrayEnemies;
	TSubclassOf<ATC_Boss> BossClass;
	TSubclassOf<ATC_Door> DoorClass;

	TArray<TSubclassOf<ATC_EnvironmentObjects>> EnvironmentObjectsArraySmallxSmall;
	TArray<TSubclassOf<ATC_EnvironmentObjects>> EnvironmentObjectsArrayBigxSmall;
	TArray<TSubclassOf<ATC_EnvironmentObjects>> EnvironmentObjectsArraySmallxBig;
	TArray<TSubclassOf<ATC_EnvironmentObjects>> EnvironmentObjectsArrayBigxBig;

	TMap<FString, UInstancedStaticMeshComponent*> MeshInstancesMap;
	int DungeonTileSize;

	ATC_GameMode* DonutGameMode;
	bool ShouldSpawnBoss = false;


	void CreateFloor();
	void CreateWalls();

	bool CheckIfCorridorInPosition(int XPos, int YPos);
	void SetEnvironmentObjects();

	void SetFinalDoor();
};
