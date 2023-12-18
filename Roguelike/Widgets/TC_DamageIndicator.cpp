// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_DamageIndicator.h"
#include "Components/TextBlock.h"

void UTC_DamageIndicator::ChangeDamageText(float Damage, FLinearColor TextColor)
{
  if (!DamageText)
    return;
  DamageText->SetText(FText::FromString(FString::FromInt(Damage)));
  DamageText->SetColorAndOpacity(TextColor);
}