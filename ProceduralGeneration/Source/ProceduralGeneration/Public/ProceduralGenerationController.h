// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Landscape.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "ProceduralGenerationController.generated.h"

USTRUCT()
struct FSpawnableObjects
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	UStaticMesh* Object;
	UPROPERTY(EditAnywhere)
	int MaxQuantity = 1;
};

UCLASS(Blueprintable)
class PROCEDURALGENERATION_API AProceduralGenerationController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProceduralGenerationController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void CreateExclusionZone();
	void SpawnObject(UStaticMesh* object);
	void ReSpawnObject(AStaticMeshActor* object);
	void DeSpawnObject(AStaticMeshActor* object);
	void DespawnObjectsFromList();
	void GenerateObjects();
	UFUNCTION(BlueprintCallable)
	void DebugDrawAllPositions();
	bool CheckObjectIsInRadius(AStaticMeshActor* object);
	bool FindNewLocationForObject(AStaticMeshActor* object);
	bool IsCollidingWithObject(AStaticMeshActor* object, FVector testPosition) const;
	FVector AddVectors(FVector &MainPosition, FVector positionToAdd, FVector* VectorToUpdate);
	UFUNCTION(BlueprintCallable, CallInEditor)
	FVector GetLocationInRangeOfPlayer();
	FVector SafeSpawnPosition();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AStaticMeshActor* Floor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* ObjectToGenerateAround;
	UPROPERTY(EditAnywhere)
	TArray<FSpawnableObjects> ObjectMeshesToSpawn;

	UPROPERTY(VisibleAnywhere)
	TArray<AStaticMeshActor*> SpawnedObjects;
	UPROPERTY(VisibleAnywhere)
	TArray<AStaticMeshActor*> InactiveObjects;
	UPROPERTY()
	TArray<AStaticMeshActor*> ObjectsToDeSpawn; //Cached Objects to be despawned

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExclusionZone;

	UPROPERTY(EditAnywhere)
	float MaxSpawnedObjects;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float MaxDistanceFromActorToSpawn;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	float MinDistBetweenSpawnedObjects;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int MaxRaycastHeight;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int MinRaycastHeight;
	UPROPERTY(EditAnywhere)
	FVector SpawningDirection;
	UPROPERTY(VisibleAnywhere)
	FVector PointOnExclusionZone;

	UPROPERTY(VisibleAnywhere)
	FVector PointOnMaxRadius;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool UseRandomDirection;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FVector DirectionToSpawn;
	FVector NewObjectSpawnLocation;
};
