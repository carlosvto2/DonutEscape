// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TC_EnemyHealth.generated.h"

class UProgressBar;

UCLASS()
class ROGUELIKE_API UTC_EnemyHealth : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UProgressBar* HealthBar;

	UFUNCTION(BlueprintCallable)
		void UpdateEnemyHealth(float Health, float MaxHealth);
};
