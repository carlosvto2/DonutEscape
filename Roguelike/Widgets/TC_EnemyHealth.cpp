// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_EnemyHealth.h"
#include "Components/ProgressBar.h"

void UTC_EnemyHealth::UpdateEnemyHealth(float Health, float MaxHealth)
{
  UE_LOG(LogTemp, Warning, TEXT("h %f"), Health);
  HealthBar->SetPercent(Health / MaxHealth);
}