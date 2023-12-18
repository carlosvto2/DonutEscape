// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TC_Shop.generated.h"

class ATC_ShopItem;

UCLASS()
class ROGUELIKE_API ATC_Shop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATC_Shop();

	void Initialize();

	UFUNCTION(BlueprintImplementableEvent)
		void GetHUDWidget();

	UFUNCTION(BlueprintImplementableEvent)
		void AddItemToShopWidget(ATC_ShopItem* ShopItem);


private:
	UPROPERTY(EditAnywhere, Category = "Shop Item")
		TArray<TSubclassOf<ATC_ShopItem>> ShopItemsArray;

	void AddShopItems();

};
