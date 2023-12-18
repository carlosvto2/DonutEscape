// Fill out your copyright notice in the Description page of Project Settings.


#include "TC_CollectableInfo.h"
#include "Components/TextBlock.h"

void UTC_CollectableInfo::ChangeCollectableText(FText CollectableInfo)
{
  if (!CollectableInfoText)
    return;
  CollectableInfoText->SetText(CollectableInfo);
}