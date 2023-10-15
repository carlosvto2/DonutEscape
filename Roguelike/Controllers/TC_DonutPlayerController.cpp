// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_DonutPlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "../Characters/TC_DonutPlayer.h"
#include "Components/CapsuleComponent.h"

void ATC_DonutPlayerController::SetPlayerEnabledState(bool bPlayerEnabled)
{
  ATC_DonutPlayer* Donut = Cast<ATC_DonutPlayer>(GetPawn());
  // enable or disable input
  if (bPlayerEnabled)
  {
    GetPawn()->EnableInput(this);
    PlayerAutomaticallyMoving = false;
    if (Donut)
    {
      Donut->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);
      Donut->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
    }
  }
  else
  {
    GetPawn()->DisableInput(this);
    PlayerAutomaticallyMoving = true;
    if (Donut)
    {
      Donut->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
      Donut->GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Ignore);
    }
  }

  // show/hide cursor
  bShowMouseCursor = true;
  
}

void ATC_DonutPlayerController::Move(FVector LocationToMove)
{
  SetPlayerEnabledState(false);
  UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, LocationToMove);
}