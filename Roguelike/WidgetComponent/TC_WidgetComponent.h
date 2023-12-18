// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "TC_WidgetComponent.generated.h"

class ATC_BaseCharacter;
class ATC_Collectable;

UCLASS()
class ROGUELIKE_API UTC_WidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeDamageWidget(ATC_BaseCharacter* Owner, float TextValue, FLinearColor TextColor);
	void InitializeEnemyHealthWidget(ATC_BaseCharacter* Owner, float Health, float MaxHealth);
	void UpdateHealthWidget(float Health, float MaxHealth);

	void InitializeCollectableInfo(FText CollectableInfo, ATC_Collectable* Owner);
	void CollectableInfoWidgetVisibility(bool Visible);

private:
	UPROPERTY(EditAnywhere)
		bool ShallBeDestroyed = true;
	UPROPERTY(EditAnywhere)
		float TimeLife;

	UPROPERTY(EditAnywhere)
		float SpeedMovementUp = 2.f;
	UPROPERTY(EditAnywhere)
		float SpeedMovementDown = 2.f;

	ATC_BaseCharacter* CharacterOwner;
	FVector InitialLocation;
	FVector LocationUpOffset = FVector(0.f, 0.f, 60.f);
	FVector MaxLocationUp;
	bool MaxLocationUpReached = false;
	float TimeLeft;
};
