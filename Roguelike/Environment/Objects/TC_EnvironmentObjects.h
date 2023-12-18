// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TC_EnvironmentObjects.generated.h"

class ATC_BaseCharacter;
class ATC_Boss;

UCLASS()
class ROGUELIKE_API ATC_EnvironmentObjects : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATC_EnvironmentObjects();
	void Initialize(bool ShouldSpawnBoss, TSubclassOf<ATC_Boss> BossClass, int DungeonTileSize,
		TArray<TSubclassOf<ATC_BaseCharacter>> ArrayEnemies);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	/* ENEMY SPAWNPOINTS*/
	UPROPERTY(EditDefaultsOnly, Category = "EnemySpawnPoints")
		USceneComponent* RoomElements;
	UPROPERTY(EditDefaultsOnly, Category = "EnemySpawnPoints")
		USceneComponent* EnemySpawnPoint;
	UPROPERTY(EditDefaultsOnly, Category = "EnemySpawnPoints")
		USceneComponent* EnemySpawnPoint2;
	UPROPERTY(EditDefaultsOnly, Category = "EnemySpawnPoints")
		USceneComponent* EnemySpawnPoint3;
	UPROPERTY(EditDefaultsOnly, Category = "EnemySpawnPoints")
		USceneComponent* EnemySpawnPoint4;

	TArray<USceneComponent*> EnemySpawnPoints;
	/* END ENEMY SPAWNPOINTS*/

	int TileSize;
	TArray<TSubclassOf<ATC_BaseCharacter>> ArrayOfEnemies;

	void SetEnemiesForRoom();
	void SetBossForRoom(TSubclassOf<ATC_Boss> BossClass);
};
