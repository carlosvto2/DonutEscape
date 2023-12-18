// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TC_Collectable.h"
#include "TC_MagicGem.generated.h"

class ATC_GameMode;

UCLASS()
class ROGUELIKE_API ATC_MagicGem : public ATC_Collectable
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	void PickUp();

private:
	ATC_GameMode* DonutGameMode;
};
