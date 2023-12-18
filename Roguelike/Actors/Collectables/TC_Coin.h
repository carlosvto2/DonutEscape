// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TC_Collectable.h"
#include "TC_Coin.generated.h"


UCLASS()
class ROGUELIKE_API ATC_Coin : public ATC_Collectable
{
	GENERATED_BODY()
	
public:

	void PickUp();
};
