// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TC_Collectable.h"
#include "TC_MultipleShoot.generated.h"


UCLASS()
class ROGUELIKE_API ATC_MultipleShoot : public ATC_Collectable
{
	GENERATED_BODY()

public:
	void PickUp();

protected:
	virtual void BeginPlay() override;
};
