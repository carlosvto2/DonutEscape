// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TC_FlameTrap.generated.h"

UCLASS()
class ROGUELIKE_API ATC_FlameTrap : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATC_FlameTrap();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* MainMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		USceneComponent* FireSpawnPoint;

	UPROPERTY(EditAnywhere, Category = "Fire")
		float FireDistance;
	UPROPERTY(EditAnywhere, Category = "Fire")
		float FireRadius;
	UPROPERTY(EditAnywhere, Category = "Fire")
		float Damage;
};
