// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TC_HealthComponent.generated.h"

class AController;
class ATC_GameMode;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ROGUELIKE_API UTC_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTC_HealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		float GetMaxHealth() { return MaxHealth; }
	UFUNCTION(BlueprintCallable)
		float GetHealth() { return Health; }

	void SetHealth(float Value);
	void SetMaxHealth(float Value);

	// Set that the health component is a shield
	void SetComponentAsShield(bool Value);

	void ToggleVisibilityHealthWidget(bool Visible);
		
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UTC_WidgetComponent> DamageWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Component", meta = (AllowPrivateAccess = "true"))
		TSubclassOf<UTC_WidgetComponent> HealthWidgetClass;


	UTC_WidgetComponent* EnemyHealthWidgetComponent;

	UPROPERTY(EditAnywhere)
		float MaxHealth = 100.0f;

	// if the health component is a shield
	bool IsShield = false;
	
	float Health = 0.0f;
	bool CharacterDead = false;

	ATC_GameMode* DonutGameMode;

	UFUNCTION() // Delegate function
		void DamageTaken(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* Instigator, AActor* DamageCauser);
	

	void CreateDamageWidgetComponent(float Damage, FLinearColor DamageColor);
};
