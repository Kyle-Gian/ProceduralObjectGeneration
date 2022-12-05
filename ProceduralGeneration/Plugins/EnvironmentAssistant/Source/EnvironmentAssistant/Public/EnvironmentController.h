// Copyright 2022, Kyle Gian, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/Actor.h"
#include "EnvironmentController.generated.h"

USTRUCT(BlueprintType)
struct FSpawnableMeshes
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, meta=(ToolTip="Static mesh to spawn"))
	UStaticMesh* Object;
	UPROPERTY(EditAnywhere, meta=(ToolTip="Quantity to spawn"))
	int MaxQuantity = 1;
	UPROPERTY(EditAnywhere, meta=(ToolTip="Only required if the static mesh origin is not on the bottom of the mesh"))
	float HeightAdjustment = 0;
};

UCLASS()
class ENVIRONMENTASSISTANT_API AEnvironmentController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvironmentController();
UFUNCTION(BlueprintCallable)
	void CreateAndPlaceItems();

	UFUNCTION(BlueprintCallable)
	void RunGenerationController();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable)
	void DebugDrawAllPositions();
	void ClearObjectArrays();
	
	
private:
	//Utilities to spawn and respawn objects
	void SpawnObject(UStaticMesh* object); // Creates a StaticMeshActor to spawn in the world
	void ReSpawnObject(AStaticMeshActor* object); // relocate object in the world
	void DeSpawnObject(AStaticMeshActor* object); //Remove object from world
	void FindLocationInExclusionRange(); //Find random location within given parameters
	void FindNewSpawnPosition();
	void DespawnObjectsFromList();
	void GenerateObjects();

	bool CheckObjectIsInRadius(AStaticMeshActor* object);
	bool FindNewLocationForObject(AStaticMeshActor* object);
	bool IsCollidingWithObject(AStaticMeshActor* object, FVector testPosition) const;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Surfaces that objects can be spawned on"))
	TArray<AStaticMeshActor*>  SpawnableSurfaces;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Used as the center position to place objects around"))
	AActor* ObjectToGenerateAround;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , meta=(ToolTip="List of static meshes that will be spawned, includes a quantity to spawn for each Mesh"))
	TArray<FSpawnableMeshes> ObjectMeshesToSpawn;

	UPROPERTY(VisibleAnywhere)
	TArray<AStaticMeshActor*> SpawnedObjects; //Currently visible in world
	UPROPERTY(VisibleAnywhere)
	TArray<AStaticMeshActor*> InactiveObjects; //objects that are inactive and are used for object pooling 
	UPROPERTY()
	TArray<AStaticMeshActor*> ObjectsToDeSpawn; //Cached Objects to be despawned

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Distance where objects can not spawn within from center position"))
	float ExclusionZone;

	UPROPERTY(EditAnywhere, meta=(ToolTip="This will be the max spawned objects during runtime"))
	float MaxSpawnedObjects;

	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta=(ToolTip="Max distance objects can spawn from the center position."))
	float MaxDistanceFromActorToSpawn;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta=(ToolTip="Min distance between spawned objects"))
	float MinDistBetweenSpawnedObjects = 50;
	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta=(ToolTip="Max height in which objects can spawn above center position"))
	int MaxSpawningHeight = 2000;
	UPROPERTY(BlueprintReadWrite,EditAnywhere , meta=(ToolTip="Min height in which objects can spawn below the center position"))
	int MinSpawningHeight = 2000;


	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta=(ToolTip="If true objects will spawn within exclusion zone (Only at start)"))
	bool SpawnInsideExclusionOnStart;
protected:
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool AllowDebug;

private:
	FVector DirectionToSpawn;
	FVector* NewObjectSpawnLocation = new FVector;

};
