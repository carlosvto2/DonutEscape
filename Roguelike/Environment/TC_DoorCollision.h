// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "TC_DoorCollision.generated.h"

class UTC_SpawnPoint;
class ATC_GameMode;
class ATC_Room;

UENUM(BlueprintType)
enum class ECollPosition : uint8
{
	Bottom UMETA(DisplayName = "Bottom Collider"),
	Top UMETA(DisplayName = "Top Collider"),
	Left UMETA(DisplayName = "Left Collider"),
	Right UMETA(DisplayName = "Right Collider")
};

UCLASS()
class ROGUELIKE_API UTC_DoorCollision : public UBoxComponent
{
	GENERATED_BODY()

public:
	UTC_DoorCollision* GetConnectedCollision() const { return ConnectedRoomCollision; }
	void SetConnectedCollision(UTC_DoorCollision* Collision) { ConnectedRoomCollision = Collision; }
	ECollPosition GetPositionInRoom() const { return PositionInRoom; }
	void SetPositionInRoom(ECollPosition Position) { PositionInRoom = Position; }
	bool GetActiveCollider() const { return ActiveCollider; }

	ATC_Room* GetRoomOwner() const { return RoomOwner; }
	void SetRoomOwner(ATC_Room* Room) { RoomOwner = Room; }

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere)
		UTC_DoorCollision* ConnectedRoomCollision;
	UPROPERTY(EditAnywhere)
		bool ActiveCollider = true;

	UFUNCTION()
		virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	ECollPosition PositionInRoom;
	ATC_GameMode* DonutGameMode;

	ATC_Room* RoomOwner;
};
