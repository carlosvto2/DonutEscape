// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TC_DamageIndicator.generated.h"

class UTextBlock;

UCLASS()
class ROGUELIKE_API UTC_DamageIndicator : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* DamageText;

	UFUNCTION(BlueprintCallable)
		void ChangeDamageText(float Damage, FLinearColor TextColor);
};
