// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_MoveComponent.h"

// Sets default values for this component's properties
UTC_MoveComponent::UTC_MoveComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTC_MoveComponent::BeginPlay()
{
	Super::BeginPlay();

	ShouldMove = false;

	OriginalLocation = GetOwner()->GetActorLocation();
	
}


// Called every frame
void UTC_MoveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	if (!ShouldMove)
		return;

	FVector CurrentLocation = MeshToMove->GetComponentLocation();
	FVector TargetLocation = OriginalLocation + MoveOffset;
	float Speed = FVector::Distance(CurrentLocation, TargetLocation) / MoveTime;

	FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, DeltaTime, Speed);

	MeshToMove->SetWorldLocation(NewLocation);
}

