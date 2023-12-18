// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TC_CollectableInfo.generated.h"

class UTextBlock;

UCLASS()
class ROGUELIKE_API UTC_CollectableInfo : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UTextBlock* CollectableInfoText;

	UFUNCTION(BlueprintCallable)
		void ChangeCollectableText(FText CollectableInfo);
};
