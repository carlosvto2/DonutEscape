// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_SpawnPoint.h"

// Sets default values for this component's properties
UTC_SpawnPoint::UTC_SpawnPoint()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTC_SpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	NeedToSpawn = true;
	
}


// Called every frame
void UTC_SpawnPoint::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

