// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "../Characters/TC_DonutPlayer.h"
#include "../Characters/TC_BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "../WidgetComponent/TC_WidgetComponent.h"
#include "../Widgets/TC_DamageIndicator.h"
#include "../Widgets/TC_EnemyHealth.h"
#include "../Widgets/TC_CollectableInfo.h"
#include "../Actors/Collectables/TC_Collectable.h"

void UTC_WidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	TimeLeft = TimeLife;
}

void UTC_WidgetComponent::InitializeDamageWidget(ATC_BaseCharacter* Owner, float TextValue, FLinearColor TextColor)
{
	CharacterOwner = Owner;
	InitialLocation = CharacterOwner->WidgetPosition->GetComponentLocation();
	MaxLocationUp = InitialLocation + LocationUpOffset;

	ATC_DonutPlayer* DonutPlayer = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!DonutPlayer)
		return;
	SetupAttachment(CharacterOwner->WidgetPosition);
	SetWidgetSpace(EWidgetSpace::World);
	SetWorldLocation(CharacterOwner->WidgetPosition->GetComponentLocation() + FVector(0.f, 0.f, 80.f));
	SetVisibility(true);
	SetCastShadow(false);
	RegisterComponent();


	UTC_DamageIndicator* DamageIndicatorWidget = Cast<UTC_DamageIndicator>(GetWidget());
	if (DamageIndicatorWidget)
		DamageIndicatorWidget->ChangeDamageText(TextValue, TextColor);
}

void UTC_WidgetComponent::InitializeEnemyHealthWidget(ATC_BaseCharacter* Owner, float Health, float MaxHealth)
{
	ATC_DonutPlayer* DonutPlayer = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!DonutPlayer)
		return;
	SetupAttachment(Owner->WidgetPosition);
	SetWidgetSpace(EWidgetSpace::World);
	SetWorldLocation(Owner->GetMesh()->GetComponentLocation() + FVector(0.f, 0.f, 80.f));
	SetVisibility(true);
	SetCastShadow(false);
	RegisterComponent();

	UpdateHealthWidget(Health, MaxHealth);
}

void UTC_WidgetComponent::UpdateHealthWidget(float Health, float MaxHealth)
{
	UTC_EnemyHealth* EnemyHealthWidget = Cast<UTC_EnemyHealth>(GetWidget());
	if (EnemyHealthWidget)
		EnemyHealthWidget->UpdateEnemyHealth(Health, MaxHealth);
}

void UTC_WidgetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	ATC_DonutPlayer* DonutPlayer = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!DonutPlayer)
		return;

	// Always rotate the component to the players camera
	FVector PlayerCameraLocation = DonutPlayer->GetPlayerCamera()->GetComponentLocation();
	FRotator RotationToCamera = UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), PlayerCameraLocation);
	RotationToCamera.Yaw = 180.f;
	SetWorldRotation(RotationToCamera);

	// If it shouldnt move and must not be destroyed avoid next part
	if (!ShallBeDestroyed)
		return;

	// if the widget reached the max location top
	if (!MaxLocationUpReached)
	{
		// If not move it up
		InitialLocation = InitialLocation + FVector(0.f, 0.f, 1.f) * DeltaTime * SpeedMovementUp;
		SetWorldLocation(InitialLocation);
		if (InitialLocation.Z >= MaxLocationUp.Z)
		{
			MaxLocationUpReached = true;
		}
	}
	else
	{
		// if it reached the max location top, move it bottom
		InitialLocation = InitialLocation - FVector(0.f, 0.f, 1.f) * DeltaTime * SpeedMovementDown;
		SetWorldLocation(InitialLocation);
	}
	

	TimeLeft -= DeltaTime;

	if (TimeLeft <= 0)
	{
		DestroyComponent();
	}
}

void UTC_WidgetComponent::InitializeCollectableInfo(FText CollectableInfo, ATC_Collectable* Owner)
{
	ATC_DonutPlayer* DonutPlayer = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!DonutPlayer)
		return;


	SetupAttachment(Owner->WidgetPosition);
	SetWidgetSpace(EWidgetSpace::World);
	SetWorldLocation(Owner->WidgetPosition->GetComponentLocation());
	SetVisibility(false);
	SetCastShadow(false);
	RegisterComponent();
	SetBlendMode(EWidgetBlendMode::Transparent);

	UTC_CollectableInfo* CollectableWidget = Cast<UTC_CollectableInfo>(GetWidget());
	if (!CollectableWidget)
		return;

	CollectableWidget->ChangeCollectableText(CollectableInfo);
}

void UTC_WidgetComponent::CollectableInfoWidgetVisibility(bool Visible)
{
	SetVisibility(Visible);
}