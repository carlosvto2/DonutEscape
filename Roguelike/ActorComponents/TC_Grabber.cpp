// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_Grabber.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

// Sets default values for this component's properties
UTC_Grabber::UTC_Grabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTC_Grabber::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTC_Grabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTC_Grabber::Grab()
{
	//UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	//if (!PhysicsHandle)
	//	return;

	//FHitResult HitResult;
	////DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 10, 10, FColor::Blue, false, 5.0f);

	//UPrimitiveComponent* HitComponent = HitResult.GetComponent();
	//// Wake rigidbodies of hit component so it can be lifted and grabbed
	//HitComponent->WakeAllRigidBodies();
	//PhysicsHandle->GrabComponentAtLocationWithRotation(
	//	HitComponent,
	//	NAME_None,
	//	HitResult.ImpactPoint,
	//	GetComponentRotation()
	//);

	//// Add grabbed tag to the actor
	//HitResult.GetActor()->Tags.Add("Grabbed");
}