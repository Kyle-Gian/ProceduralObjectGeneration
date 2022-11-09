// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ProceduralGenerationHUD.generated.h"

UCLASS()
class AProceduralGenerationHUD : public AHUD
{
	GENERATED_BODY()

public:
	AProceduralGenerationHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

