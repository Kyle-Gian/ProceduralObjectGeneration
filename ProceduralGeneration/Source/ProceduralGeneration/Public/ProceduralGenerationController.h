// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Landscape.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "ProceduralGenerationController.generated.h"

USTRUCT(BlueprintType)
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
	
	UFUNCTION(BlueprintCallable, CallInEditor)
	void CreateAndPlaceItems();

	UFUNCTION(BlueprintCallable, CallInEditor)
	void RunGenerationController();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void DebugDrawAllPositions();
	
	
private:
	//Utilities to spawn and respawn objects
	void SpawnObject(UStaticMesh* object);
	void ReSpawnObject(AStaticMeshActor* object);
	void DeSpawnObject(AStaticMeshActor* object);
	void FindLocationInExclusionRange();
	void SafeSpawnPosition();
	void DespawnObjectsFromList();
	void GenerateObjects();

	bool CheckObjectIsInRadius(AStaticMeshActor* object);
	bool FindNewLocationForObject(AStaticMeshActor* object);
	bool IsCollidingWithObject(AStaticMeshActor* object, FVector testPosition) const;
	FVector AddVectors(FVector &MainPosition, FVector positionToAdd, FVector* VectorToUpdate);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AStaticMeshActor* Floor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* ObjectToGenerateAround;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
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
	float MinDistBetweenSpawnedObjects = 50;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int MaxSpawningHeight = 2000;
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	int MinSpawningHeight = 2000;


	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool SpawnInsideExclusionOnStart;
protected:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool AllowDebug;

private:
	FVector DirectionToSpawn;
	FVector* NewObjectSpawnLocation = new FVector;

};
