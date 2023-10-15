// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TC_MoveComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROGUELIKE_API UTC_MoveComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTC_MoveComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UStaticMeshComponent* MeshToMove;
	bool GetShouldMove() { return ShouldMove; }
	void SetShouldMove(bool Move) { ShouldMove = Move; }

private:
	UPROPERTY(EditAnywhere)
		FVector MoveOffset;
	UPROPERTY(EditAnywhere)
		float MoveTime = 5.f;

	UPROPERTY(EditAnywhere)
		bool ShouldMove = false;
	FVector OriginalLocation;
};
