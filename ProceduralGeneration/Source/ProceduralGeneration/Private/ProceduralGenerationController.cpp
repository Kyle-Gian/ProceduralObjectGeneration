// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralGenerationController.h"

#include <string>

#include "DrawDebugHelpers.h"

// Sets default values
AProceduralGenerationController::AProceduralGenerationController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AProceduralGenerationController::BeginPlay()
{
	Super::BeginPlay();
	CreateAndPlaceItems();
}

void AProceduralGenerationController::SpawnObject(UStaticMesh* object)
{
	if (!object)
		return;
	
	AStaticMeshActor* newActor = GetWorld()->SpawnActor<AStaticMeshActor>();
	newActor->GetStaticMeshComponent()->SetStaticMesh(object);
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
	object->SetActorHiddenInGame(false);

	if (FindNewLocationForObject(object))
	{
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
	if (!AllowDebug)
		return;
	
	if (ObjectToGenerateAround)
	{
		FindLocationInExclusionRange();
		DrawDebugSphere(GetWorld(), ObjectToGenerateAround->GetActorLocation(), ExclusionZone,16,FColor::Red, true);
		DrawDebugSphere(GetWorld(), ObjectToGenerateAround->GetActorLocation(), MaxDistanceFromActorToSpawn,16,FColor::Red, true);
		DrawDebugSphere(GetWorld(), *NewObjectSpawnLocation, 10,16,FColor::Blue, true);
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
	bool itemPlacementFailed = true;
	while (itemPlacementFailed)
	{
		maxTries--;

		//SafePositionNotFound
		if (maxTries <= 0)
			return false;
		
		SafeSpawnPosition();

		if (!NewObjectSpawnLocation->IsZero())
			itemPlacementFailed = IsCollidingWithObject(object, *NewObjectSpawnLocation);
	}
	
	object->SetActorLocation(*NewObjectSpawnLocation);
	return true;
}

bool AProceduralGenerationController::IsCollidingWithObject(AStaticMeshActor* object, FVector testPosition) const
{
	FVector positionToCheck = testPosition;
	FVector ObjectBoxExtents = object->GetStaticMeshComponent()->GetStaticMesh()->GetExtendedBounds().BoxExtent;
	//Check if the mesh will collider with other objects
	TArray<FOverlapResult> hitActors;
	GetWorld()->OverlapMultiByChannel(hitActors, positionToCheck, object->GetActorQuat(), ECC_WorldStatic,
	                                  FCollisionShape::MakeBox(
		                                  ObjectBoxExtents + MinDistBetweenSpawnedObjects));
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
	FVector newPosition = FVector::ZeroVector;

	if (positionToAdd.X > 0 && MainPosition.X < 0)
	{
		newPosition.X = abs(MainPosition.X) + positionToAdd.X;
	}
	else
	{
		newPosition.X = MainPosition.X + positionToAdd.X;
	}
	
	if (positionToAdd.Y > 0 && MainPosition.Y < 0)
	{
		newPosition.Y = abs(MainPosition.Y) + positionToAdd.Y;
	}
	else
	{
		newPosition.Y = MainPosition.Y + positionToAdd.Y;
	}

	if (positionToAdd.Z > 0 && MainPosition.Z < 0)
	{
		newPosition.Z = abs(MainPosition.Z) + positionToAdd.Z;
	}
	else
	{
		newPosition.Z = MainPosition.Z + positionToAdd.Z;
	}


	return newPosition;
	
}

void AProceduralGenerationController::FindLocationInExclusionRange()
{
	FVector MiddleLocation = ObjectToGenerateAround->GetActorLocation();
	

	float maxDirVal = 1;
	DirectionToSpawn.X = FMath::FRandRange(-maxDirVal, maxDirVal);
	maxDirVal -= abs(DirectionToSpawn.X);
	DirectionToSpawn.Y = FMath::FRandRange(-maxDirVal, maxDirVal);
	DirectionToSpawn.Z = 0;
	
	DirectionToSpawn.Normalize();

	float RandDistBetweenExclusions = FMath::RandRange(ExclusionZone, MaxDistanceFromActorToSpawn);
	FVector SpawnPosition = (DirectionToSpawn * RandDistBetweenExclusions);
	*NewObjectSpawnLocation = AddVectors(MiddleLocation,SpawnPosition ,NewObjectSpawnLocation);
	NewObjectSpawnLocation->Z = MiddleLocation.Z;
	if (FVector::Distance(MiddleLocation, *NewObjectSpawnLocation) < ExclusionZone)
	{
		UE_LOG(LogTemp, Warning, TEXT("Text, %s %s %s"),
	   *MiddleLocation.ToString(),*SpawnPosition.ToString(),*NewObjectSpawnLocation->ToString());
	}
}


void AProceduralGenerationController::SafeSpawnPosition()
{
	FHitResult hitResult;
	FindLocationInExclusionRange();
	FVector EndLocation = FVector(NewObjectSpawnLocation->X,NewObjectSpawnLocation->Y,NewObjectSpawnLocation->Z - MinSpawningHeight);
	
	NewObjectSpawnLocation->Z += MaxSpawningHeight;
	//DrawDebugLine(GetWorld(),LocationToCheck,EndLocation,FColor::Red,false,5);
	if (GetWorld()->LineTraceSingleByChannel(hitResult,*NewObjectSpawnLocation, EndLocation,ECC_WorldStatic))
	{
		NewObjectSpawnLocation->Z = hitResult.Location.Z;
		return;
	}

	*NewObjectSpawnLocation = FVector::ZeroVector;
}

void AProceduralGenerationController::CreateAndPlaceItems()
{
	//If Spawn onstart is true store the original exclusion value to set back after items generated
	float tempExclusion = ExclusionZone;
	if (SpawnInsideExclusionOnStart)
	{
		tempExclusion = ExclusionZone;
		ExclusionZone = 200;
	}
	
	GenerateObjects();
	
	ExclusionZone = tempExclusion;
}

void AProceduralGenerationController::RunGenerationController()
{
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
				}

			}
		
		}
	}
}

// Called every frame
void AProceduralGenerationController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	RunGenerationController();
}

