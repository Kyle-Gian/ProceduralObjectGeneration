// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "Road.generated.h"

UCLASS()
class PROCEDURALGENERATION_API ARoad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARoad();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(CallInEditor,BlueprintCallable, Category = "SplineEditorTool")
	void AdjustSplineToMeshWithDistance();
	UFUNCTION(CallInEditor,BlueprintCallable, Category = "SplineEditorTool")
	void AdjustSplineToMeshWithNodePositions();
	FVector GetHitPositionOnMesh(FVector PositionToCheck);

	UPROPERTY(EditAnywhere)
	float distanceForSplineAdjustment = 1000;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere)
	USplineComponent* PathSpline;

	

};
