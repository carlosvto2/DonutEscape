// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_Door.h"
#include "Components/StaticMeshComponent.h"
#include "../ActorComponents/TC_MoveComponent.h"

// Sets default values
ATC_Door::ATC_Door()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Main = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh"));
	RootComponent = Main;
	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(Main);
	IlluminationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IlluminationMesh"));
	IlluminationMesh->SetupAttachment(Main);

	// Add health
	MoveComponent = CreateDefaultSubobject<UTC_MoveComponent>(TEXT("Move"));
	MoveComponent->MeshToMove = BaseMesh;
}

// Called when the game starts or when spawned
void ATC_Door::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATC_Door::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

