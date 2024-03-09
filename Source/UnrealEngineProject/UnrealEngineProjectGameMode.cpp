// Copyright Epic Games, Inc. All Rights Reserved.

#include "UnrealEngineProjectGameMode.h"
#include "UnrealEngineProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUnrealEngineProjectGameMode::AUnrealEngineProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
