// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TC_DungeonCorridor.generated.h"

UCLASS()
class ROGUELIKE_API ATC_DungeonCorridor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATC_DungeonCorridor();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnDungeonCorridor(FVector Start, FVector End, 
		TMap<FString, UInstancedStaticMeshComponent*> InstancesMeshMap, int TileSize);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	TMap<FString, UInstancedStaticMeshComponent*> MeshInstancesMap;
	int DungeonTileSize;
	FVector CorridorStartPosition;
	FVector CorridorEndPosition;
	UInstancedStaticMeshComponent* InstancedWallComponent;
	FRotator WallRotation1;
	FRotator WallRotation2;

	void GetCorrectStartEndAxis(int& StartX, int& StartY, int& EndX, int& EndY, bool CheckCorner);

	void SpawnCorridorsWalls();

	void SetWallsDependingOnCorner(int x, int y);
	void CorridorCornerWallRotation(int x, int y, FRotator WallRotationXAxis, FRotator WallRotationYAxis);
};
