// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_FlameTrap.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "../../Characters/TC_DonutPlayer.h"

// Sets default values
ATC_FlameTrap::ATC_FlameTrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainMesh"));
	RootComponent = MainMesh;

	FireSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Fire Spawn Point"));
	FireSpawnPoint->SetupAttachment(MainMesh);

}

// Called when the game starts or when spawned
void ATC_FlameTrap::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATC_FlameTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));
	TArray<AActor*, FDefaultAllocator> IgnoreActors;
	FHitResult OutHit;

	FVector StartPosition = FireSpawnPoint->GetComponentLocation();
	FVector EndPosition = FireSpawnPoint->GetForwardVector() * FireDistance + StartPosition;
	UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), StartPosition,
		EndPosition, FireRadius, ObjectTypesArray, false, IgnoreActors, EDrawDebugTrace::None,
		OutHit, true, FLinearColor::Red);

	if (OutHit.bBlockingHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("hit %s"), *OutHit.GetActor()->GetName());
		// if the trap hit the player
		ATC_DonutPlayer* DonutPlayer = Cast<ATC_DonutPlayer>(OutHit.GetActor());
		if (DonutPlayer)
		{
			UGameplayStatics::ApplyDamage(DonutPlayer, Damage * DeltaTime, NULL, this, UDamageType::StaticClass());
		}
	}
}

