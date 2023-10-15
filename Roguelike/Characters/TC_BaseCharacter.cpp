// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_BaseCharacter.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Actors/TC_Projectile.h"
#include "../Actors/Collectables/TC_Collectable.h"
#include "../ActorComponents/TC_HealthComponent.h"
#include "TC_DonutPlayer.h"
#include "../TC_GameMode.h"
#include "../Environment/TC_Room.h"

// Sets default values
ATC_BaseCharacter::ATC_BaseCharacter()
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(RootComponent);

	// Add health
	Health = CreateDefaultSubobject<UTC_HealthComponent>(TEXT("Health"));
}

// Called when the game starts or when spawned
void ATC_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	bCharacterActive = false;

	DonutGameMode = Cast<ATC_GameMode>(UGameplayStatics::GetGameMode(this));
	if (!DonutGameMode)
		return;
}

void ATC_BaseCharacter::HandleDestruction()
{
	// Visual/sound effects
	if (!RoomOwner)
		return;

}

void ATC_BaseCharacter::RotateMesh(FVector LookAtTarget)
{
	// Destination - StartPoint
	FVector ToTarget = LookAtTarget - GetActorLocation();
	// Get direction where the vector is pointing
	FRotator LookAtRotation = ToTarget.Rotation();
	LookAtRotation.Pitch = 0.f;

	// Interpolate between current rotation and target rotation, to go smoother
	SetActorRotation(FMath::RInterpTo(
		GetActorRotation(),
		LookAtRotation,
		UGameplayStatics::GetWorldDeltaSeconds(this),
		25.0f)
	);	
}

void ATC_BaseCharacter::Fire()
{
	if (!bCanFire || !bCharacterActive || !ProjectileClass)
		return;

	// Only if character is player
	ATC_DonutPlayer* DonutPlayer = Cast<ATC_DonutPlayer>(this);

	if (DonutPlayer)
	{
		DonutPlayer->IncrementOverheat();
		// GameMode will handle overheat with widget
		DonutGameMode->HandlePlayerOverheat();
	}

	if (!bMultipleShoot)
	{
		ATC_Projectile* Projectile = GetWorld()->SpawnActor<ATC_Projectile>(ProjectileClass,
			ProjectileSpawnPoint->GetComponentLocation(),
			ProjectileSpawnPoint->GetComponentRotation());
		// So the projectile know who is the owner
		Projectile->SetOwner(this);
	}
	else
	{
		// if the player has the buff to shoot two projectiles

		ATC_Projectile* Projectile = GetWorld()->SpawnActor<ATC_Projectile>(ProjectileClass,
			ProjectileSpawnPoint->GetComponentLocation(),
			ProjectileSpawnPoint->GetComponentRotation());
		Projectile->AddActorLocalOffset(FVector(0.f, 25.f, 0.f));
		// So the projectile know who is the owner
		Projectile->SetOwner(this);

		ATC_Projectile* Projectile2 = GetWorld()->SpawnActor<ATC_Projectile>(ProjectileClass,
			ProjectileSpawnPoint->GetComponentLocation(),
			ProjectileSpawnPoint->GetComponentRotation());
		Projectile2->AddActorLocalOffset(FVector(0.f, -25.f, 0.f));
		// So the projectile know who is the owner
		Projectile2->SetOwner(this);
	}

	
}

void ATC_BaseCharacter::SpawnCollectable()
{
	// if no collectables or it was already spawned
	if (Collectables.Num() == 0 || bCollectableAlreadySpawned)
		return;

	int RandomCollectable = FMath::RandRange(0, 2);

	// just get collectable if value is 0
	if (RandomCollectable > 0)
		return;

	RandomCollectable = FMath::RandRange(0, Collectables.Num() - 1);
	if (!Collectables[RandomCollectable])
		return;


	GetWorld()->SpawnActor<ATC_Collectable>(Collectables[RandomCollectable], GetActorLocation(), FRotator(0.f, 0.f, 0.f));
	bCollectableAlreadySpawned = true;
}