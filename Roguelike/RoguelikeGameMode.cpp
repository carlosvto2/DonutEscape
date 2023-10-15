// Copyright Epic Games, Inc. All Rights Reserved.

#include "RoguelikeGameMode.h"
#include "RoguelikeCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARoguelikeGameMode::ARoguelikeGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
