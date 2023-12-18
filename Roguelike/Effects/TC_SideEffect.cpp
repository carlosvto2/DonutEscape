// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_SideEffect.h"
#include "../Characters/TC_BaseCharacter.h"
#include "../ActorComponents/TC_StatusComponent.h"
#include "../TC_GameMode.h"

ATC_SideEffect::ATC_SideEffect()
{
  // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
  PrimaryActorTick.bCanEverTick = true;
}

void ATC_SideEffect::BeginPlay()
{
  Super::BeginPlay();

  DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));

  if (!DonutGameMode)
    return;

  DurationLeft = EffectDuration;

  IncrementStacks();
}

// Called every frame
void ATC_SideEffect::Tick(float DeltaTime)
{
  DurationLeft -= DeltaTime;

  // Update the side effect in widget
  if (DonutGameMode)
  {
    DonutGameMode->UpdateSideEffectWidget(EffectTexture, EffectDuration, DurationLeft);
  }

  // if the duration is over, deactivate effect
  if (DurationLeft <= 0)
  {
    DeactivateEffect();
  }
}

void ATC_SideEffect::IncrementStacks()
{
  Stacks++;
  DurationLeft = EffectDuration;
}

void ATC_SideEffect::DeactivateEffect()
{
  ATC_BaseCharacter* CharacterOwner = Cast<ATC_BaseCharacter>(GetOwner());
  if (!CharacterOwner)
    return;

  UTC_StatusComponent* StatusComponent = CharacterOwner->GetStatusComponent();
  if (!StatusComponent)
    return;

  Stacks = 0;
  StatusComponent->RemoveSideEffect(this);

  Destroy();
}