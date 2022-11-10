// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralGenerationController.h"

#include <string>

#include "DrawDebugHelpers.h"

// Sets default values
AProceduralGenerationController::AProceduralGenerationController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	UseRandomDirection = true;
}

// Called when the game starts or when spawned
void AProceduralGenerationController::BeginPlay()
{
	Super::BeginPlay();
	CreateExclusionZone();
	GenerateObjects();
}

void AProceduralGenerationController::CreateExclusionZone()
{

}

void AProceduralGenerationController::SpawnObject(UStaticMesh* object)
{
	AStaticMeshActor* newActor = GetWorld()->SpawnActor<AStaticMeshActor>();
	newActor->SetMobility(EComponentMobility::Movable);
	newActor->GetStaticMeshComponent()->SetStaticMesh(object);
	
	if (SpawnedObjects.Num() < MaxSpawnedObjects)
	{
		ReSpawnObject(newActor);
		return;
	}
	
	DeSpawnObject(newActor);

}

void AProceduralGenerationController::ReSpawnObject(AStaticMeshActor* object)
{

	InactiveObjects.Remove(object);
	
	object->SetActorEnableCollision(true);
	
	if (FindNewLocationForObject(object))
	{
		object->SetActorHiddenInGame(false);
		SpawnedObjects.Add(object);
		return;
	}
	DeSpawnObject(object);
}

void AProceduralGenerationController::DeSpawnObject(AStaticMeshActor* object)
{

	SpawnedObjects.Remove(object);
	
	object->SetActorEnableCollision(false);
	object->SetActorHiddenInGame(true);
	InactiveObjects.Add(object);
}

void AProceduralGenerationController::DespawnObjectsFromList()
{
	for (auto object : ObjectsToDeSpawn)
	{
		DeSpawnObject(object);
	}
	ObjectsToDeSpawn.Empty();
}

void AProceduralGenerationController::GenerateObjects()
{
		for (auto SpawnableObjects : ObjectMeshesToSpawn)
		{
			for (int i = 0; i < SpawnableObjects.MaxQuantity; i++)
			{
				SpawnObject(SpawnableObjects.Object);
			}
		}
}

void AProceduralGenerationController::DebugDrawAllPositions()
{
	FlushPersistentDebugLines(GetWorld());
	if (ObjectToGenerateAround)
	{
		GetLocationInRangeOfPlayer();
		DrawDebugSphere(GetWorld(), ObjectToGenerateAround->GetActorLocation(), ExclusionZone,16,FColor::Red, true);
		DrawDebugSphere(GetWorld(), ObjectToGenerateAround->GetActorLocation(), MaxDistanceFromActorToSpawn,16,FColor::Red, true);
		DrawDebugLine(GetWorld(),ObjectToGenerateAround->GetActorLocation(),PointOnExclusionZone,FColor::Blue, true,100, 0, 10);
		DrawDebugLine(GetWorld(),ObjectToGenerateAround->GetActorLocation(),PointOnMaxRadius,FColor::Orange, true);
		DrawDebugSphere(GetWorld(), NewObjectSpawnLocation, 10,16,FColor::Blue, true);
	}
}

bool AProceduralGenerationController::CheckObjectIsInRadius(AStaticMeshActor* object)
{
	return FVector::Distance(ObjectToGenerateAround->GetActorLocation(), object->GetActorLocation()) < MaxDistanceFromActorToSpawn;
}

bool AProceduralGenerationController::FindNewLocationForObject(AStaticMeshActor* object)
{
	//TODO: If we break out of loop Do not spawn object
	float maxTries = 10;
	FVector newLocation = SafeSpawnPosition();
	while (IsCollidingWithObject(object, newLocation) || newLocation.IsZero())
	{
		maxTries--;
		//SafePositionNotFound
		if (maxTries <= 0)
		{
			return false;
		}
		
		newLocation = SafeSpawnPosition();
	}
	
	object->SetActorLocation(newLocation);
	return true;
}

bool AProceduralGenerationController::IsCollidingWithObject(AStaticMeshActor* object, FVector testPosition) const
{
	//Check if the mesh will collider with other objects
	TArray<FOverlapResult> hitActors;
	GetWorld()->OverlapMultiByChannel(hitActors, testPosition, object->GetActorQuat(),ECC_WorldStatic,FCollisionShape::MakeBox(object->GetStaticMeshComponent()->GetStaticMesh()->GetExtendedBounds().BoxExtent + MinDistBetweenSpawnedObjects));
	for (auto HitActor : hitActors)
	{
		if (Cast<AStaticMeshActor>(HitActor.GetActor()) || Cast<UStaticMeshComponent>(HitActor.GetComponent()))
		{
			if (HitActor.GetActor() != Floor)
			{
				return true;
			}
		}
	}
	return false;
}

FVector AProceduralGenerationController::AddVectors(FVector &MainPosition, FVector positionToAdd,FVector* VectorToUpdate)
{
	float xVal;
	float yVal;
	float zVal;

	if (positionToAdd.X > 0 && MainPosition.X < 0)
	{
		xVal = +MainPosition.X + positionToAdd.X;
	}
	else
	{
		xVal = MainPosition.X + positionToAdd.X;
	}
	
	if (positionToAdd.Y > 0 && MainPosition.Y < 0)
	{
		yVal = +MainPosition.Y + positionToAdd.Y;
	}
	else
	{
		yVal = MainPosition.Y + positionToAdd.Y;
	}

	if (positionToAdd.Z > 0 && MainPosition.Z < 0)
	{
		zVal = +MainPosition.Z + positionToAdd.Z;
	}
	else
	{
		zVal = MainPosition.Z + positionToAdd.Z;
	}

	
	*VectorToUpdate = FVector(xVal,yVal,zVal);

	return FVector(xVal,yVal,zVal);
	
}

FVector AProceduralGenerationController::GetLocationInRangeOfPlayer()
{
	FVector MiddleLocation = ObjectToGenerateAround->GetActorLocation();
	
	if (UseRandomDirection)
	{
		float maxDirVal = 1;
		DirectionToSpawn.X = FMath::FRandRange(-maxDirVal, maxDirVal);
		maxDirVal -= abs(DirectionToSpawn.X);
		DirectionToSpawn.Y = FMath::FRandRange(-maxDirVal, maxDirVal);
		maxDirVal -= abs(DirectionToSpawn.Y);
		DirectionToSpawn.Z = FMath::FRandRange(0, 0);
	}
	DirectionToSpawn.Normalize();
	PointOnExclusionZone.Z = MiddleLocation.Z;

	float RandDistBetweenExclusions = FMath::RandRange(ExclusionZone, MaxDistanceFromActorToSpawn);
	FVector SpawnPosition = (DirectionToSpawn * RandDistBetweenExclusions);
	AddVectors(MiddleLocation,SpawnPosition ,&NewObjectSpawnLocation);
	NewObjectSpawnLocation.Z = MiddleLocation.Z;
	if (FVector::Distance(MiddleLocation, NewObjectSpawnLocation) < ExclusionZone)
	{
		UE_LOG(LogTemp, Warning, TEXT("Text, %s %s %s"),
	   *MiddleLocation.ToString(),*SpawnPosition.ToString(),*NewObjectSpawnLocation.ToString());
	}

	
	return NewObjectSpawnLocation;
}

FVector AProceduralGenerationController::SafeSpawnPosition()
{
	FHitResult hitResult;
	FVector LocationToCheck = GetLocationInRangeOfPlayer();
	FVector EndLocation = FVector(LocationToCheck.X,LocationToCheck.Y,LocationToCheck.Z - MinRaycastHeight);
	LocationToCheck.Z += MaxRaycastHeight;
	//DrawDebugLine(GetWorld(),LocationToCheck,EndLocation,FColor::Red,false,5);
	if (GetWorld()->LineTraceSingleByChannel(hitResult,LocationToCheck, EndLocation,ECC_WorldStatic))
	{
		LocationToCheck.Z = hitResult.Location.Z;
		return LocationToCheck;
	}

	return FVector::ZeroVector;
}

// Called every frame
void AProceduralGenerationController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (auto object : SpawnedObjects)
	{
		if (CheckObjectIsInRadius(object))
		{
			continue;
		}
		ObjectsToDeSpawn.Add(object);
	}
	if (ObjectsToDeSpawn.Num() > 0)
	{
		DespawnObjectsFromList();
	
		//Try and refill the Objects that are respawned to max size
		float ItemsToTrySpawn = MaxSpawnedObjects - SpawnedObjects.Num();
		//GEngine->AddOnScreenDebugMessage(INDEX_NONE,5.0f, FColor::Red, FString::SanitizeFloat(ItemsToTrySpawn));
		if (ItemsToTrySpawn > 0)
		{
			for (int i = 0; i <= ItemsToTrySpawn; i++)
			{
				if (i < InactiveObjects.Num())
				{
					AStaticMeshActor* newObject = InactiveObjects[FMath::FRandRange(0, InactiveObjects.Num() - 1)];
					ReSpawnObject(newObject);
					//if (!MoveObjectInsidePlayerArea(newObject))
					//{
						//DeSpawnObject(newObject);
					//}
				}

			}
		
		}
	}
	
}

