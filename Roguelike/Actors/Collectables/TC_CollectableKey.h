// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TC_Collectable.h"
#include "TC_CollectableKey.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKE_API ATC_CollectableKey : public ATC_Collectable
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
