// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TC_Collectable.h"
#include "TC_MagicBox.generated.h"

class ATC_GameMode;

UCLASS()
class ROGUELIKE_API ATC_MagicBox : public ATC_Collectable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATC_MagicBox();

	void PickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	ATC_GameMode* DonutGameMode;
};
