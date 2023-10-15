// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_BasePawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"

// Sets default values
ATC_BasePawn::ATC_BasePawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Collider"));
	RootComponent = CapsuleComp;

	BaseMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Base Skeletal Mesh"));
	BaseMesh->SetupAttachment(CapsuleComp);

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(BaseMesh);
}

// Called when the game starts or when spawned
void ATC_BasePawn::BeginPlay()
{
	Super::BeginPlay();
	
}

