// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TC_GameMode.generated.h"

class ATC_DonutPlayer;
class ATC_DonutPlayerController;
class ATC_Room;
class UTC_SpawnPoint;
class ATC_Door;
class UTC_DoorCollision;
class ATC_MagicBox;
class ATC_BaseCharacter;
class ATC_Boss;
class ATC_MultipleShoot;
class USoundWave;
class ATC_Collectable;
class ATC_DungeonGenerator;
class ATC_EnvironmentObjects;
class UWidgetComponent;
class ATC_Shop;

UCLASS()
class ROGUELIKE_API ATC_GameMode : public AGameMode
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		void StartGame();


public:
	UPROPERTY(EditAnywhere, Category = "Enemies")
		TArray<TSubclassOf<ATC_BaseCharacter>> EnemiesArray;
	UPROPERTY(EditAnywhere, Category = "EnvironmentObjects")
		TArray<TSubclassOf<ATC_EnvironmentObjects>> EnvironmentObjectsArraySmallxSmall;
	UPROPERTY(EditAnywhere, Category = "EnvironmentObjects")
		TArray<TSubclassOf<ATC_EnvironmentObjects>> EnvironmentObjectsArrayBigxSmall;
	UPROPERTY(EditAnywhere, Category = "EnvironmentObjects")
		TArray<TSubclassOf<ATC_EnvironmentObjects>> EnvironmentObjectsArraySmallxBig;
	UPROPERTY(EditAnywhere, Category = "EnvironmentObjects")
		TArray<TSubclassOf<ATC_EnvironmentObjects>> EnvironmentObjectsArrayBigxBig;

	UPROPERTY(EditAnywhere, Category = "Boss")
		TSubclassOf<ATC_Boss> BossClass;
	UPROPERTY(EditAnywhere, Category = "LastDoor")
		TSubclassOf<ATC_Door> DoorClass;

	UFUNCTION(BlueprintImplementableEvent)
		void GameOver(bool bWonGame);

	// Offset for the player to spawn in each room position
	UPROPERTY(EditAnywhere, Category = "Spawn Offsets")
		FVector BottomPlayerOffset = FVector(50.f, 0.f, 0.f);
	UPROPERTY(EditAnywhere, Category = "Spawn Offsets")
		FVector TopPlayerOffset = FVector(-50.f, 0.f, 0.f);
	UPROPERTY(EditAnywhere, Category = "Spawn Offsets")
		FVector RightPlayerOffset = FVector(0.f, -50.f, 0.f);
	UPROPERTY(EditAnywhere, Category = "Spawn Offsets")
		FVector LeftPlayerOffset = FVector(0.f, 50.f, 0.f);

	void HandleMapRotation();
	void ActorDied(AActor* DeadActor);
	//void MovePlayerToRoom(AActor* ActorOverlapped, UTC_DoorCollision* ConnectedDoorCollision);

	/* BLUEPRINT IMPLEMENTABLE FUNCTIONS */
	UFUNCTION(BlueprintImplementableEvent)
		void MultipleShootWidget();
	UFUNCTION(BlueprintImplementableEvent)
		void UpdatePlayerHealthWidget();
	UFUNCTION(BlueprintImplementableEvent)
		void UpdatePlayerShieldWidget();
	UFUNCTION(BlueprintImplementableEvent)
		void UpdatePlayerBombsWidget();
	UFUNCTION(BlueprintImplementableEvent)
		void BlackScreenFadeInOut(bool Forward);
	UFUNCTION(BlueprintImplementableEvent)
		void ShowCollectableButtonWidget(bool bShow);
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateBossHealthWidget(bool bShow, ATC_Boss* Boss);
	UFUNCTION(BlueprintImplementableEvent)
		void ShowHelpTextWidget(const FString& Text);

	// PROJECTILES
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateProjectileRangeColor(bool InRange, float Opacity);
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateProjectileTypeWidget(EProjectile ProjectileType);
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateNumberOfProjectiles(int FireMaxProj, int WaterMaxProj, int PlantMaxProj);
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateNumberOfProjectilesLeftWidget(EProjectile ProjectileType, int ProjectilesLeft,
			int MaxProjectiles);

	// SIDE EFFECTS
	UFUNCTION(BlueprintImplementableEvent)
		void AddSideEffectWidget(UTexture2D* SideEffectTexture, int NumberStacks);
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateSideEffectWidget(UTexture2D* SideEffectTexture, float Duration, float DurationLeft);
	UFUNCTION(BlueprintImplementableEvent)
		void RemoveSideEffectWidget(UTexture2D* SideEffectTexture);

	// POWER UNITS
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateSkinsPowerUnits(int FirePowerUnits, int MaxPowerUnits, int WaterPowerUnits,
			int MaxWaterPowerUnits, int PlantPowerUnits, int MaxPlantPowerUnits);
	UFUNCTION(BlueprintImplementableEvent)
		void UpdatePowerUnitsValue(int PowerUnits);
	UFUNCTION(BlueprintImplementableEvent)
		void ToggleGemShop(ESlateVisibility Visible);
	/* END BLUEPRINT IMPLEMENTABLE FUNCTIONS */

private:
	UPROPERTY(EditAnywhere, Category = "Maze")
		bool CreateMaze = true;

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundWave* GameSound;
	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundWave* MoveRoomsSound;

	/* ROOM ARRAYS */
	UPROPERTY(EditAnywhere, Category = "Rooms")
		TArray<TSubclassOf<ATC_Room>> BottomRooms;
	UPROPERTY(EditAnywhere, Category = "Rooms")
		TArray<TSubclassOf<ATC_Room>> TopRooms;
	UPROPERTY(EditAnywhere, Category = "Rooms")
		TArray<TSubclassOf<ATC_Room>> RightRooms;
	UPROPERTY(EditAnywhere, Category = "Rooms")
		TArray<TSubclassOf<ATC_Room>> LeftRooms;
	/* END ROOM ARRAYS */

	/* SUBCLASSES */
	UPROPERTY(EditAnywhere, Category = "Rooms")
		TSubclassOf<ATC_Room> InitialRoomClass;
	UPROPERTY(EditAnywhere, Category = "LastDoor")
		TSubclassOf<ATC_MultipleShoot> BuffClass;
	UPROPERTY(EditAnywhere, Category = "LastDoor")
		TSubclassOf<ATC_MagicBox> MagicBoxClass;
	UPROPERTY(EditAnywhere, Category = "Collectables")
		TArray<TSubclassOf<ATC_Collectable>> Collectables;
	/* END SUBCLASSES */

	/* DUNGEON GENERATOR*/
	ATC_DungeonGenerator* DungeonGenerator;
	UPROPERTY(EditAnywhere, Category = "Dungeon Generator")
		int DungeonSize;
	UPROPERTY(EditAnywhere, Category = "Dungeon Generator")
		int MaxRoomSize;
	UPROPERTY(EditAnywhere, Category = "Dungeon Generator")
		int MinRoomSize;
	UPROPERTY(EditAnywhere, Category = "Dungeon Generator")
		int MaxNumberOfRooms;
	UPROPERTY(EditAnywhere, Category = "Dungeon Generator")
		int NumberOfRetryPlacingRoom;
	UPROPERTY(EditAnywhere, Category = "Dungeon Generator")
		float TileSize;
	/* END DUNGEON GENERATOR*/


	/* SHOP */
	UPROPERTY(EditAnywhere, Category = "Shop")
		TSubclassOf<ATC_Shop> ShopClass;
	/* END SHOP */
	
	ATC_Room* InitialRoom;

	/* PLAYER FUNCTIONS */
	//void HandleRoomVisibility(ATC_Room* RoomToShow, TArray<ATC_Room*> RoomsToHide);
	//void HandlePlayerRoomsWhenEnterNewRoom(ATC_Room* RoomToShow, TArray<ATC_Room*> RoomsToHide);
	/* END PLAYER FUNCTIONS */

	/* ROTATION FUNCTIONS */
	void InitializeRotationDegreesArray();
	/* END ROTATION FUNCTIONS */

	/* ROOM FUNCTIONS */
	//void SetRooms();
	void SetFinalDoor();
	void SetMultipleShootBuff();
	void SetBoss();
	//void SetSideRooms(ATC_Room* ActualRoom);
	//void SpawnRoom(ATC_Room* ActualRoom, UTC_SpawnPoint* RoomSpawnPoint);

	//bool CheckIfRoomAlreadyInSpawnpoint(UTC_SpawnPoint* SpawnPointToCheck);
	/* END ROOM FUNCTIONS */

	/* COLLIDER FUNCTIONS */
	//void ConnectRoomColliders();
	//void SaveCollidersArray();
	//void SetObjectInAllUnusedActivatedColliders();
	/* END COLLIDER FUNCTIONS */

	/* ENEMY FUNCTIONS */
	//void HandleEnemiesInRoom(ATC_Room* NewRoom, ATC_Room* OldRoom);
	/* END ENEMY FUNCTIONS */

	void HandleGameStart();

	ATC_DonutPlayer* Donut;
	ATC_DonutPlayerController* DonutPlayerController;

	/* ARRAY VARIABLES */
	TArray<ATC_Room*> RoomArray;
	TArray<UTC_SpawnPoint*> SpawnPointArray;
	TArray<UTC_DoorCollision*> DoorCollisionArray;
	TArray<float> TurnDegrees;
	/* END ARRAY VARIABLES */

	int RandomRotationAndWallTransparency = 0;
	ATC_Door* LastDoor;
	ATC_Boss* EnemyBoss;

	float StartDelay = 3.0f;
	//float MovementDelay = 2.0f;


};
