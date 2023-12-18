// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TC_Collectable.h"
#include "TC_PowerUnit.generated.h"

class UTC_WidgetComponent;

UCLASS()
class ROGUELIKE_API ATC_PowerUnit : public ATC_Collectable
{
	GENERATED_BODY()

public:
	void PickUp();


};
