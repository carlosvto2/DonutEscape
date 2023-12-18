// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TC_Collectable.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;
class ATC_DonutPlayer;

UCLASS()
class ROGUELIKE_API ATC_Collectable : public AActor
{
	GENERATED_BODY()
	
public:	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
		USceneComponent* WidgetPosition;
	// Sets default values for this actor's properties
	ATC_Collectable();
	virtual void BeginPlay() override;

	void CollectableWidgetVisibility(bool Visible);

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Collectable Info", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UTC_WidgetComponent> CollectableInfoWidgetClass;

	UPROPERTY(VisibleAnywhere, Category = "Collectable Info")
		UTC_WidgetComponent* CollectableWidgetComponent;
	UPROPERTY(EditAnywhere, Category = "Collectable Info")
		FText CollectableInfo;

	UStaticMeshComponent* GetCollectableMesh() const { return CollectableMesh; };
	ATC_DonutPlayer* Donut;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, Category = "Sound")
		USoundWave* PickUpSound;

	// Get if actor Active
	bool IsActive() const;
	// Set if actor active
	void SetActive(bool CollectableActive);

	void PickUp();

private:
	UPROPERTY(EditDefaultsOnly, Category = "Collectables")
		UStaticMeshComponent* CollectableMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Illumination")
		UPointLightComponent* Light;

	UPROPERTY(EditDefaultsOnly, Category = "WidgetPosition")
		float WidgetPositionZValue = 200.f;

	bool Active = true;

	UPROPERTY(EditAnywhere, Category = "Rotation")
		float RotationSpeed = 10.f;
	UPROPERTY(EditAnywhere, Category = "Rotation")
		FRotator RotationDirection = FRotator(0.f, 1.f, 0.f);

};
