// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_BaseCharacter.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"
#include "../Actors/TC_Projectile.h"
#include "../Actors/Collectables/TC_Collectable.h"
#include "../ActorComponents/TC_HealthComponent.h"
#include "../ActorComponents/TC_StatusComponent.h"
#include "TC_DonutPlayer.h"
#include "../TC_GameMode.h"
#include "../Environment/TC_Room.h"

// Sets default values
ATC_BaseCharacter::ATC_BaseCharacter(const FObjectInitializer& OI) : Super(OI)
{
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ProjectileSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Projectile Spawn Point"));
	ProjectileSpawnPoint->SetupAttachment(RootComponent);

	WidgetPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Widget Position"));
	WidgetPosition->SetupAttachment(RootComponent);

	// Add Health Component
	Health = OI.CreateDefaultSubobject<UTC_HealthComponent>(this, TEXT("Health"));
	StatusComponent = OI.CreateDefaultSubobject<UTC_StatusComponent>(this, TEXT("Status Component"));
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

/*
* Function to get the status component created in blueprint
*/
void ATC_BaseCharacter::SetStatusComponent(UTC_StatusComponent* Component)
{
	// Set Status Component
	StatusComponent = Component;
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

void ATC_BaseCharacter::OnPlayerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ATC_DonutPlayer* DonutPlayer = Cast<ATC_DonutPlayer>(UGameplayStatics::GetPlayerPawn(this, 0));
	if (!DonutPlayer)
		return;

	// if the overlapped actor is the player, activate the enemy
	if (Other == DonutPlayer)
	{
		ActivateEnemy(true);
	}
}



/*
* IF CHARACTER IS PLAYER - Check the type of projectile and skin and apply side effects to PLAYER
*/
void ATC_BaseCharacter::CalculateAmmountOfDamageToCharacter(ATC_Projectile* Projectile, float& TotalDamage)
{
	ATC_DonutPlayer* DonutPlayer = Cast<ATC_DonutPlayer>(this);

	if (DonutPlayer)
	{
		// if the player was hit
		TotalDamage = StatusComponent->CalculateAmmountOfDamageToPlayer(Projectile);
	}
	else
	{
		// if an enemy was hit
		TotalDamage = StatusComponent->CalculateAmmountOfDamageToEnemy(Projectile);
	}
}

/*
* Use counter attack depending on characters skin
*/
void ATC_BaseCharacter::UseCounterAttack()
{
	switch (CharacterSkin)
	{
		case ESkin::Fire:
			CounterFireAttackToPlantProjectile();
			break;
		case ESkin::Water:
			CounterWaterAttackToFireProjectile();
			break;
		case ESkin::Plant:
			CounterPlantAttackToWaterProjectile();
			break;
	}
}

/*
* Create timer to make damage through time
*/
void ATC_BaseCharacter::MakeDamageThroughTime()
{
	// we need a timer delegate because the function to call has input parameters
	FTimerDelegate FireBurnDamageDelegate = FTimerDelegate::CreateUObject(
		this,
		&ATC_BaseCharacter::MakeBurningDamage,
		1.f
	);
	GetWorldTimerManager().SetTimer(FireBurnDamageTimer, FireBurnDamageDelegate, 1.0f, true);
}

/*
* Make damage to this actor
*/
void ATC_BaseCharacter::MakeBurningDamage(float Damage)
{
	// if the time of burning is not done
	if (BurningTimeCounter < BurningTimeMax)
	{
		AController* ControllerInstigator = GetInstigatorController();
		if (ControllerInstigator)
		{
			// apply the damage to the other actor
			UGameplayStatics::ApplyDamage(this, Damage, ControllerInstigator, this, UDamageType::StaticClass());

		}
		BurningTimeCounter++;
	}
	else
	{
		// clear burning timer
		GetWorldTimerManager().ClearTimer(FireBurnDamageTimer);
		BurningTimeCounter = 0;
	}
	
}