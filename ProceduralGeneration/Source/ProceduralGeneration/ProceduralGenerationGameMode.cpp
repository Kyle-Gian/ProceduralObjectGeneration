// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralGenerationGameMode.h"
#include "ProceduralGenerationHUD.h"
#include "ProceduralGenerationCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProceduralGenerationGameMode::AProceduralGenerationGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AProceduralGenerationHUD::StaticClass();
}
