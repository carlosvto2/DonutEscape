// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TC_Collectable.h"
#include "TC_BombCollectable.generated.h"


UCLASS()
class ROGUELIKE_API ATC_BombCollectable : public ATC_Collectable
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	void PickUp();

private:
	UPROPERTY(EditAnywhere, Category = "Bomb")
		int NumberOfBombsIncrement = 2;

};
