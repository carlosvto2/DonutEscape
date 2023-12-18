// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TC_Room.generated.h"

class UTC_SpawnPoint;
class UTC_DoorCollision;
class ATC_BaseCharacter;
class ATC_Door;
class ATC_Boss;
class ATC_MultipleShoot;
class ATC_GameMode;
class USoundWave;
class ATC_Collectable;

UCLASS()
class ROGUELIKE_API ATC_Room : public AActor
{
	GENERATED_BODY()
	
public:	

	/* ROOM SPAWNPOINTS */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rooms")
		USceneComponent* RoomElements;
	UPROPERTY(EditDefaultsOnly, Category = "RoomSpawnPoint")
		UTC_SpawnPoint* BottomRoomSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "RoomSpawnPoint")
		UTC_SpawnPoint* TopRoomSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "RoomSpawnPoint")
		UTC_SpawnPoint* RightRoomSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "RoomSpawnPoint")
		UTC_SpawnPoint* LeftRoomSpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "RoomSpawnPoint")
		UTC_SpawnPoint* SpawnPointInitialRoom;
	/* END ROOM SPAWNPOINTS */
	/* ROOM DOOR COLLISIONS */
	UPROPERTY(EditDefaultsOnly, Category = "RoomColliders")
		UTC_DoorCollision* BottomRoomCollision;
	UPROPERTY(EditDefaultsOnly, Category = "RoomColliders")
		UTC_DoorCollision* TopRoomCollision;
	UPROPERTY(EditDefaultsOnly, Category = "RoomColliders")
		UTC_DoorCollision* RightRoomCollision;
	UPROPERTY(EditDefaultsOnly, Category = "RoomColliders")
		UTC_DoorCollision* LeftRoomCollision;
	/* END ROOM DOOR COLLISIONS */
	/* ENEMY SPAWNPOINTS*/
	UPROPERTY(EditDefaultsOnly, Category = "EnemySpawnPoints")
		USceneComponent* EnemySpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "EnemySpawnPoints")
		USceneComponent* EnemySpawnPoint2;
	UPROPERTY(EditDefaultsOnly, Category = "EnemySpawnPoints")
		USceneComponent* EnemySpawnPoint3;
	UPROPERTY(EditDefaultsOnly, Category = "EnemySpawnPoints")
		USceneComponent* EnemySpawnPoint4;
	/* END ENEMY SPAWNPOINTS*/

	// Sets default values for this actor's properties
	ATC_Room();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	TArray<UTC_SpawnPoint*> GetSpawnPoints() { return SpawnPoints; }
	TArray<UTC_DoorCollision*> GetRoomColliders() { return RoomColliders; }
	UTC_DoorCollision* GetBottomCollider() { return BottomRoomCollision; }
	TArray<USceneComponent*> GetEnemySpawnPoints() { return EnemySpawnPoints; }

	void HideRoomAndElements(bool bShow);

	/* ENEMY FUNCTIONS */
	void SpawnRoomElements(TArray<TSubclassOf<ATC_BaseCharacter>> EnemiesArray,
		TArray<TSubclassOf<ATC_Collectable>> Collectables);
	void TryToSpawnCollectable(TArray<TSubclassOf<ATC_Collectable>> Collectables, USceneComponent* SpawnPoint);
	void TryToSpawnEnemiesAndActivate(TArray<TSubclassOf<ATC_BaseCharacter>> EnemiesArray, USceneComponent* SpawnPoint);

	void DeleteAllEnemiesInRoom();
	bool GetIfAllEnemiesKilled() const { return AllEnemiesKilled; }
	void SetAllEnemiesKilled(bool Value) { AllEnemiesKilled = Value; }
	void CheckIfLastEnemy(ATC_BaseCharacter* EnemyKilled);
	/* END ENEMY FUNCTIONS */

	/* FINAL DOOR FUNCTIONS */
	ATC_Door* GetIfHasFinalDoor() const { return FinalDoor; }
	void SetFinalDoor(ATC_Door* Door) { FinalDoor = Door; }
	/* END FINAL DOOR FUNCTIONS */

	void CreateBuff(TSubclassOf<ATC_MultipleShoot> BuffClass);

	/* FINAL BOSS FUNCTIONS */
	void SaveFinalBoss(ATC_Boss* Boss);
	/* END FINAL BOSS FUNCTIONS */
	
	void DeactivateSpawnPointForRoom(int SpawnPointPosition);

	void ChangeWallMaterialsAfterRotation(USoundWave* MoveRoomSound, int TransparentWallMovement);

	void NoBuffInRoom() { Buff = nullptr; };
	void NoBossInRoom() { FinalBoss = nullptr; };
	bool CheckIfBossCanBeInThisRoom() { return BossCanBeInThisRoom; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:

	UPROPERTY(EditAnywhere, Category = "Material")
		UMaterialInstance* WallMaterial;
	UPROPERTY(EditAnywhere, Category = "Material")
		UMaterialInstance* WallMaterialTransparent;
	UPROPERTY(EditAnywhere, Category = "Material")
		UMaterialInstance* WindowMaterial;
	UPROPERTY(EditAnywhere, Category = "Material")
		UMaterialInstance* WindowMaterialTransparent;

	UPROPERTY(EditAnywhere, Category = "Boss")
		bool BossCanBeInThisRoom = true;

	TArray<UTC_SpawnPoint*> SpawnPoints;
	TArray<UTC_DoorCollision*> RoomColliders;
	TArray<USceneComponent*> EnemySpawnPoints;
	TArray<ATC_BaseCharacter*> Enemies;

	// Array to get the walls that need to be transparent
	TArray<FName> TransparentWallMaterials;
	int PartWallsWithTransparentMaterial;

	ATC_GameMode* DonutGameMode;
	ATC_Door* FinalDoor;
	ATC_Boss* FinalBoss;
	ATC_MultipleShoot* Buff;

	bool FinalBossSet = false;
	bool AllEnemiesKilled = false;

	void SetRoomSpawnPoints();
	void SetRoomColliders();
	void SetEnemySpawnPoints();

	void GetArrayOfTags(TArray<FName> &ArrayTags, int TransparentWallMovement);
	void SetWallsMaterialArray();

};
