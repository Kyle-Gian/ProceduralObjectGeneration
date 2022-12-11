// Copyright 2022, Kyle Gian, All rights reserved


#include "EnvironmentController.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Landscape.h"

// Sets default values
AEnvironmentController::AEnvironmentController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEnvironmentController::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnvironmentController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnvironmentController::SpawnObject(FSpawnableMeshesEditorView* object)
{
	if (!object)
		return;
	AStaticMeshActor* newActor = GetWorld()->SpawnActor<AStaticMeshActor>();
	newActor->GetStaticMeshComponent()->SetStaticMesh(object->Object);
	newActor->SetMobility(EComponentMobility::Movable);
	newActor->GetStaticMeshComponent()->SetStaticMesh(object->Object);

	FGeneratedObjects* newObject = new FGeneratedObjects(newActor,object->HeightAdjustment);


	if (SpawnedObjects.Num() < MaxSpawnedObjects)
	{
		ReSpawnObject(newObject);
		return;
	}
	//No spawn location was found Despawn the Mesh or reached max spawned items
	DeSpawnObject(newObject);
}

void AEnvironmentController::ReSpawnObject(FGeneratedObjects* object)
{
	InactiveObjects.Remove(object);
	//Enabled after Spawn
	object.Object->SetActorEnableCollision(true);
	object.Object->SetActorHiddenInGame(false);

	if (FindNewLocationForObject(object))
	{
		SpawnedObjects.Add(object);
		return;
	}
	//No spawn location was found Despawn the Mesh
	DeSpawnObject(object);
}

void AEnvironmentController::DeSpawnObject(FGeneratedObjects* object)
{
	SpawnedObjects.Remove(object);
	object->SetActorEnableCollision(false);
	object->SetActorHiddenInGame(true);
	InactiveObjects.Add(object);
}

void AEnvironmentController::DespawnObjectsFromList()
{
	for (auto object : ObjectsToDeSpawn)
	{
		DeSpawnObject(object);
	}
	ObjectsToDeSpawn.Empty();
}

void AEnvironmentController::GenerateObjects()
{
	//Create static mesh actors
	for (auto SpawnableObjects : ObjectMeshesToSpawn)
	{
		for (int i = 0; i < SpawnableObjects.MaxQuantity; i++)
		{
			SpawnObject(&SpawnableObjects);
		}
	}
}

void AEnvironmentController::DebugDrawAllPositions()
{
	FlushPersistentDebugLines(GetWorld());
	if (!AllowDebug)
		return;

	if (ObjectToGenerateAround)
	{
		FindLocationInExclusionRange();
		DrawDebugSphere(GetWorld(), ObjectToGenerateAround->GetActorLocation(), ExclusionZone, 16, FColor::Red, true);
		DrawDebugSphere(GetWorld(), ObjectToGenerateAround->GetActorLocation(), MaxDistanceFromActorToSpawn, 16,
		                FColor::Red, true);
		DrawDebugSphere(GetWorld(), *NewObjectSpawnLocation, 10, 16, FColor::Blue, true);
	}
}

void AEnvironmentController::ClearObjectArrays()
{
	for (auto object : SpawnedObjects)
	{
		object->Destroy();
	}
	SpawnedObjects.Empty();

	for (auto object : ObjectsToDeSpawn)
	{
		object->Destroy();
	}
	ObjectsToDeSpawn.Empty();
}

bool AEnvironmentController::CheckObjectIsInRadius(AStaticMeshActor* object)
{
	return FVector::Distance(ObjectToGenerateAround->GetActorLocation(), object->GetActorLocation()) <
		MaxDistanceFromActorToSpawn;
}

bool AEnvironmentController::FindNewLocationForObject(AStaticMeshActor* object)
{
	// Try and find a location to spawn the object at, if None are found do not spawn and add to non spawned list
	float maxTries = 10;
	bool itemPlacementFailed = true;
	while (itemPlacementFailed)
	{
		maxTries--;

		//SafePositionNotFound after so many tries
		if (maxTries <= 0)
			return false;
		//Find new position
		FindNewSpawnPosition();
		
		//Check position is not colliding with other objects
		if (!NewObjectSpawnLocation->IsZero())
			itemPlacementFailed = IsCollidingWithObject(object, *NewObjectSpawnLocation);
	}

	object->SetActorLocation(*NewObjectSpawnLocation);
	return true;
}

bool AEnvironmentController::IsCollidingWithObject(AStaticMeshActor* object, FVector testPosition) const
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
		for (auto surface : SpawnableSurfaces)
		{
			if (HitActor.GetActor() == surface || Cast<ALandscape>(HitActor.GetActor()))
			{
				return false;
			}
		}
	}
	return true;
}

void AEnvironmentController::FindLocationInExclusionRange()
{
	FVector MiddleLocation = ObjectToGenerateAround->GetActorLocation();
	//Create random direction and normalize
	float maxDirVal = 1;
	DirectionToSpawn.X = FMath::FRandRange(-maxDirVal, maxDirVal);
	DirectionToSpawn.Y = FMath::FRandRange(-maxDirVal, maxDirVal);
	DirectionToSpawn.Z = 0;

	DirectionToSpawn.Normalize();

	//Get a distance value between the player exclusion and the max spawn exclusion
	float RandDistBetweenExclusions = FMath::RandRange(ExclusionZone, MaxDistanceFromActorToSpawn);
	FVector SpawnPosition = (DirectionToSpawn * RandDistBetweenExclusions);
	*NewObjectSpawnLocation = MiddleLocation + SpawnPosition;
	NewObjectSpawnLocation->Z = MiddleLocation.Z;
	
		UE_LOG(LogTemp, Warning, TEXT("Text, %s %s %s"),
		       *MiddleLocation.ToString(), *SpawnPosition.ToString(), *NewObjectSpawnLocation->ToString());
}


void AEnvironmentController::FindNewSpawnPosition()
{
	FHitResult hitResult;
	FindLocationInExclusionRange();
	FVector EndLocation = FVector(NewObjectSpawnLocation->X, NewObjectSpawnLocation->Y,
	                              NewObjectSpawnLocation->Z - MinSpawningHeight);

	NewObjectSpawnLocation->Z += MaxSpawningHeight;
	//DrawDebugLine(GetWorld(),LocationToCheck,EndLocation,FColor::Red,false,5);
	if (GetWorld()->LineTraceSingleByChannel(hitResult, *NewObjectSpawnLocation, EndLocation, ECC_WorldStatic))
	{
		NewObjectSpawnLocation->Z = hitResult.Location.Z;
		return;
	}

	*NewObjectSpawnLocation = FVector::ZeroVector;
}

void AEnvironmentController::CreateAndPlaceItems()
{
	//Clear any potential remaining objects in array
	ClearObjectArrays();
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

void AEnvironmentController::RunGenerationController()
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
