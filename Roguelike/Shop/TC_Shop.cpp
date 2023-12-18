// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_Shop.h"
#include "TC_ShopItem.h"

// Sets default values
ATC_Shop::ATC_Shop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATC_Shop::Initialize()
{
	GetHUDWidget();
	AddShopItems();
}


void ATC_Shop::AddShopItems()
{
	for (TSubclassOf<ATC_ShopItem> ShopItemClass : ShopItemsArray)
	{
		// Create the items of the shop
		ATC_ShopItem* ItemShop = NewObject<ATC_ShopItem>(this, ShopItemClass);
		AddItemToShopWidget(ItemShop);
		ItemShop->Destroy();
	}
}