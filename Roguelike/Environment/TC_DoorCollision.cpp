// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_DoorCollision.h"
#include "../TC_GameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"

void UTC_DoorCollision::BeginPlay()
{
  Super::BeginPlay();

  DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));
  if (!DonutGameMode)
    return;

  OnComponentBeginOverlap.AddDynamic(this, &UTC_DoorCollision::OnBeginOverlap);
}

void UTC_DoorCollision::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
  if (!GetActiveCollider())
    return;

  USphereComponent* SphereComponent = Cast<USphereComponent>(OtherComp);

  // if spherecomponent not continue (this is the sphere collector of the character
  if (SphereComponent)
    return;

  //DonutGameMode->MovePlayerToRoom(Other, this);
}