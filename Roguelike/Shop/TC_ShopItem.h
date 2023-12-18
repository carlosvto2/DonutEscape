// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TC_ShopItem.generated.h"

class UTexture2D;

UCLASS()
class ROGUELIKE_API ATC_ShopItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATC_ShopItem();

	UFUNCTION(BlueprintCallable)
		UTexture2D* GetItemImage() const { return ItemImage; }
	UFUNCTION(BlueprintCallable)
		FText GetItemName() const { return ItemName; }
	UFUNCTION(BlueprintCallable)
		FText GetItemDescription() const { return ItemDescription; }
	UFUNCTION(BlueprintCallable)
		int GetPowerUnitsCost() const { return PowerUnitsCost; }
	UFUNCTION(BlueprintCallable)
		int GetMoneyCost() const { return MoneyCost; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
		UTexture2D* ItemImage;

	UPROPERTY(EditAnywhere)
		FText ItemName;
	UPROPERTY(EditAnywhere)
		FText ItemDescription;

	UPROPERTY(EditAnywhere)
		int PowerUnitsCost;
	UPROPERTY(EditAnywhere)
		int MoneyCost;

};
