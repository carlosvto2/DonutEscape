// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TC_DonutPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKE_API ATC_DonutPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	void SetPlayerEnabledState(bool bPlayerEnabled);
	void Move(FVector LocationToMove);
	bool GetIfPlayerAutomaticallyMoving() const { return PlayerAutomaticallyMoving; }

private:
	bool PlayerAutomaticallyMoving = false;
};
