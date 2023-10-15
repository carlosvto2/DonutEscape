// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_Donut.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"

ATC_Donut::ATC_Donut()
{
  SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
  SpringArm->SetupAttachment(RootComponent);

  Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
  Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ATC_Donut::BeginPlay()
{
  Super::BeginPlay();
  // get the controller
  DonutController = Cast<APlayerController>(GetController());
}

void ATC_Donut::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
  PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ATC_Donut::Move);
  PlayerInputComponent->BindAxis(TEXT("Turn"), this, &ATC_Donut::Turn);
}

void ATC_Donut::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);
}

void ATC_Donut::Move(float Value)
{
  // this function is called every frame, because axis mapping are called every frame
  // use the value (1.f or -1.f) to move the tank forward or back
  FVector DeltaLocation = FVector::ZeroVector;
  DeltaLocation.X = Value * UGameplayStatics::GetWorldDeltaSeconds(this) * Speed;
  AddActorLocalOffset(DeltaLocation, true);
}

void ATC_Donut::Turn(float Value)
{
  FRotator DeltaRotation = FRotator::ZeroRotator;
  // Yaw = Value * DeltaTime * TurnRate
  DeltaRotation.Yaw = Value * UGameplayStatics::GetWorldDeltaSeconds(this) * TurnRate;
  AddActorLocalRotation(DeltaRotation, true);
}