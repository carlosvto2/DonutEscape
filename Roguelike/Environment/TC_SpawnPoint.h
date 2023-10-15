// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "TC_SpawnPoint.generated.h"

class ATC_Room;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROGUELIKE_API UTC_SpawnPoint : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTC_SpawnPoint();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool GetNeedToSpawn() const { return NeedToSpawn; }
	void SetNeedToSpawn(bool Value) { NeedToSpawn = Value; }

	int GetOpeningDirection() const { return OpeningDirection; }

	ATC_Room* GetRoomOwner() const { return RoomOwner; }
	void SetRoomOwner(ATC_Room* Room) { RoomOwner = Room; }

private:
	// 1 --> need bottom door
	// 2 --> need top door
	// 3 --> need left door
	// 4 --> need right door
	UPROPERTY(EditAnywhere, Category = "Direction")
		int OpeningDirection;

	bool NeedToSpawn = true;
		
	ATC_Room* RoomOwner;
};
