// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_Collectable.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PointLightComponent.h"
#include "../../WidgetComponent/TC_WidgetComponent.h"

// Sets default values
ATC_Collectable::ATC_Collectable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollectableMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collectable Mesh"));
	RootComponent = CollectableMesh;
	WidgetPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Widget Position"));
	WidgetPosition->SetupAttachment(CollectableMesh);

	Light = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));
	Light->SetupAttachment(CollectableMesh);
}

void ATC_Collectable::BeginPlay()
{
	Super::BeginPlay();
	// Set health widget for Enemy
	if (CollectableInfoWidgetClass)
	{
		CollectableWidgetComponent = NewObject<UTC_WidgetComponent>(this, CollectableInfoWidgetClass);
		CollectableWidgetComponent->InitializeCollectableInfo(CollectableInfo, this);
	}
}

// Called every frame
void ATC_Collectable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator DeltaRotation = RotationDirection * RotationSpeed * UGameplayStatics::GetWorldDeltaSeconds(this);
	FRotator FinalRotation = CollectableMesh->GetComponentRotation() + DeltaRotation;
	// Continously rotate the collectable
	CollectableMesh->SetWorldRotation(FinalRotation);

	// Update the position of the widget if the position is not correct
	if (WidgetPosition->GetComponentLocation().Z < WidgetPositionZValue - 20.f
		|| WidgetPosition->GetComponentLocation().Z > WidgetPositionZValue + 20.f)
	{
		WidgetPosition->SetWorldLocation(FVector(GetActorLocation().X, GetActorLocation().Y, WidgetPositionZValue));
	}
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

/*
* Set the collectable widget visibility
*/
void ATC_Collectable::CollectableWidgetVisibility(bool Visible)
{
	if (!CollectableWidgetComponent)
		return;
	CollectableWidgetComponent->CollectableInfoWidgetVisibility(Visible);
}