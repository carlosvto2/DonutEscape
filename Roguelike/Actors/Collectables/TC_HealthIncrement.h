// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TC_Collectable.h"
#include "TC_HealthIncrement.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKE_API ATC_HealthIncrement : public ATC_Collectable
{
	GENERATED_BODY()

public:
	void PickUp();

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Health")
		float HealthIncrementation = 10.f;
};
