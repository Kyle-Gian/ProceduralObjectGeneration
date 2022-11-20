// Fill out your copyright notice in the Description page of Project Settings.


#include "Road.h"

// Sets default values
ARoad::ARoad()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PathSpline = CreateDefaultSubobject<USplineComponent>("Path");

}

// Called when the game starts or when spawned
void ARoad::BeginPlay()
{
	Super::BeginPlay();
}

void ARoad::AdjustSplineToMeshWithDistance()
{
	AdjustSplineToMeshWithNodePositions();
	float splineLength = PathSpline->GetSplineLength();
	float currentDistanceToCheck = distanceForSplineAdjustment;
	int currentSplinePointPos = 0;

	while (currentDistanceToCheck <= splineLength)
	{
		FVector positionOnSpline = PathSpline->GetWorldLocationAtDistanceAlongSpline(currentDistanceToCheck);
		FVector hitLocation = GetHitPositionOnMesh(positionOnSpline);

		if (hitLocation == FVector::ZeroVector)
		{
			currentDistanceToCheck += distanceForSplineAdjustment;
			continue;
		}

		if (hitLocation.Z > positionOnSpline.Z + 10 || hitLocation.Z < positionOnSpline.Z + 10 )
		{
			const int numberOfPoints = PathSpline->GetNumberOfSplinePoints();

			for (currentSplinePointPos; currentSplinePointPos <= numberOfPoints - 1; currentSplinePointPos++)
			{
				float currentNodeDistAlongSpline = PathSpline->GetDistanceAlongSplineAtSplinePoint(currentSplinePointPos);
				if (currentNodeDistAlongSpline > currentDistanceToCheck)
				{
					if (abs(currentNodeDistAlongSpline - currentDistanceToCheck) > distanceForSplineAdjustment && abs(PathSpline->GetDistanceAlongSplineAtSplinePoint(currentSplinePointPos + 1) - currentDistanceToCheck) > distanceForSplineAdjustment)
					{
						hitLocation.Z += 10;
						PathSpline->AddSplinePointAtIndex(hitLocation, currentSplinePointPos, ESplineCoordinateSpace::World,
														  true);
					}
					break;
				}
				
			}
		}
		currentDistanceToCheck += distanceForSplineAdjustment;
	}
}

void ARoad::AdjustSplineToMeshWithNodePositions()
{
	int SplinePoints = PathSpline->GetNumberOfSplinePoints();
	for(int i = 0; i < SplinePoints; i++)
	{
		const FVector currentSplinePos = PathSpline->GetWorldLocationAtSplinePoint(i);
		
		FVector hitLocation = GetHitPositionOnMesh(currentSplinePos);

		if (hitLocation.Z > currentSplinePos.Z + 10 || hitLocation.Z < currentSplinePos.Z + 10 )
		{
			hitLocation.Z += 10;
			PathSpline->SetLocationAtSplinePoint(i,hitLocation, ESplineCoordinateSpace::World);
		}
	}
}

FVector ARoad::GetHitPositionOnMesh(FVector positionToCheck)
{
	FHitResult hitResult;
	FVector startRayPosition, endRayPosition;
	startRayPosition = positionToCheck;
	endRayPosition = positionToCheck;

	endRayPosition.Z -= 1000.0f;
	startRayPosition.Z += 1000.0f;
	if (GetWorld()->LineTraceSingleByChannel(hitResult, startRayPosition, endRayPosition, ECC_Visibility))
	{
		return hitResult.Location;
	}
	return FVector::ZeroVector;
}

// Called every frame
void ARoad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
