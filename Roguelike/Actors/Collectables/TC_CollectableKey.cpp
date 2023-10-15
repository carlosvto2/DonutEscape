// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_CollectableKey.h"

// Called when the game starts or when spawned
void ATC_CollectableKey::BeginPlay()
{
	Super::BeginPlay();

	GetCollectableMesh()->SetWorldRotation(FRotator(0.f, 0.f, 90.f));
}