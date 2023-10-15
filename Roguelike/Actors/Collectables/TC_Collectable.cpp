// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_Collectable.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PointLightComponent.h"

// Sets default values
ATC_Collectable::ATC_Collectable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollectableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collectable Mesh"));
	RootComponent = CollectableMesh;
	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetupAttachment(CollectableMesh);
}

// Called every frame
void ATC_Collectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator DeltaRotation = RotationDirection * RotationSpeed * UGameplayStatics::GetWorldDeltaSeconds(this);
	FRotator FinalRotation = CollectableMesh->GetComponentRotation() + DeltaRotation;
	// Continously rotate the collectable
	CollectableMesh->SetWorldRotation(FinalRotation);
}

bool ATC_Collectable::IsActive() const
{
	return Active;
}

void ATC_Collectable::SetActive(bool CollectableActive)
{
	Active = CollectableActive;
}

void ATC_Collectable::PickUp()
{
	SetActive(false);
	Destroy();

	if (PickUpSound)
	{
		UGameplayStatics::PlaySound2D(this, PickUpSound, 1.0f, 1.0f);
	}
}