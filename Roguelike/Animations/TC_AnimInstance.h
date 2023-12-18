// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "TC_AnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKE_API UTC_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UTC_AnimInstance(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grab)
		bool GrabItem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Grab)
		bool ThrowItem;

	void GrabAnimation();
	void ThrowAnimation();
};
