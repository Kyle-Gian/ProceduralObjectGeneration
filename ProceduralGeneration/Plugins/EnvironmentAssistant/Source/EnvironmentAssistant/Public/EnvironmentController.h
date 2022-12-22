// Copyright 2022, Kyle Gian, All rights reserved

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentObject.h"
#include "Landscape.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Actor.h"
#include "EnvironmentController.generated.h"

USTRUCT(BlueprintType)
struct FSpawnableMeshes
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, meta=(ToolTip="Static mesh to spawn"), Category="SpawnableObject")
	UStaticMesh* Object;
	UPROPERTY(EditAnywhere, meta=(ToolTip="Quantity to spawn"), Category="SpawnableObject")
	int MaxQuantity = 1;
	UPROPERTY(EditAnywhere, meta=(ToolTip="Only required if the static mesh origin is not on the bottom of the mesh"), Category="SpawnableObject")
	float HeightAdjustment = 0;
};

USTRUCT(BlueprintType)
struct FSpawnableEnvironmentObject
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, meta=(ToolTip="Class type to spawn"), Category="SpawnableClass")
	TSubclassOf<AEnvironmentObject> ClassToSpawn;
	UPROPERTY(EditAnywhere, meta=(ToolTip="Quantity to spawn"), Category="SpawnableClass")
	int MaxQuantity = 1;
	UPROPERTY(EditAnywhere, meta=(ToolTip="Only required if the static mesh origin is not on the bottom of the mesh"), Category="SpawnableObject")
	float HeightAdjustment = 0;
};

UCLASS()
class ENVIRONMENTASSISTANT_API AEnvironmentController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvironmentController();
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	UFUNCTION(BlueprintCallable, Category="EnvironmentAssistant")
	void CreateAndPlaceItems();

	UFUNCTION(BlueprintCallable,Category="EnvironmentAssistant")
	void RunGenerationController();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintCallable, Category="EnvironmentAssistant/Debug")
	void DebugDrawAllPositions();
	void ClearObjectArrays();
	
	
private:
	//Utilities to spawn and respawn objects
	void SpawnObjectFromMesh(FSpawnableMeshes* object); // Creates a StaticMeshActor to spawn in the world
	void SpawnObjectFromClass(FSpawnableEnvironmentObject* object); // Creates a StaticMeshActor to spawn in the world
	void ReSpawnObject(AEnvironmentObject* object); // relocate object in the world
	void DeSpawnObject(AEnvironmentObject* object); //Remove object from world
	void FindLocationInExclusionRange(); //Find random location within given parameters
	void FindNewSpawnPosition();
	void DespawnObjectsFromList();
	void GenerateObjects();

	bool CheckObjectIsInRadius(AEnvironmentObject* object);
	bool FindNewLocationForObject(AEnvironmentObject* object);
	bool IsCollidingWithObject(AEnvironmentObject* object, FVector testPosition) const;
	void ValidateValues();
	void ConvertObjectsToStringArray();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Surfaces that objects can be spawned on"), Category="EnvironmentAssistant|Zoning")
	TArray<AStaticMeshActor*>  SpawnableSurfaces;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Surfaces that objects can be spawned on"), Category="EnvironmentAssistant|Zoning")
	TArray<ALandscape*>  SpawnableLandscapes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Used as the center position to place objects around"), Category="EnvironmentAssistant|Zoning")
	AActor* ObjectToGenerateAround;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , meta=(ToolTip="List of static meshes that will be spawned, includes a quantity to spawn for each Mesh"), Category="EnvironmentAssistant|Spawning")
	TArray<FSpawnableMeshes> ObjectMeshesToSpawn;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category="EnvironmentAssistant|Spawning", meta=(ToolTip="Any class added to this array must derive from EnvironmentObject to be included"))
	TArray<FSpawnableEnvironmentObject> EnvironmentObjectsToSpawn; //Currently visible in world


	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ToolTip="Distance where objects can not spawn within from center position", ClampMin="0",UIMin="0"), Category="EnvironmentAssistant|Zoning")
	float ExclusionZone = 3000;

	UPROPERTY(EditAnywhere, meta=(ToolTip="This will be the max spawned objects during runtime", ClampMin="0",UIMin="0"), Category="EnvironmentAssistant|Spawning")
	float MaxSpawnedObjects = 100;

	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta=(ToolTip="Max distance objects can spawn from the center position.", ClampMin="0",UIMin="0"), Category="EnvironmentAssistant|Zoning")
	float MaxDistanceFromActorToSpawn = 500;
	
	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta=(ToolTip="Min distance between spawned objects", ClampMin="0",UIMin="0"), Category="EnvironmentAssistant|Zoning")
	float MinDistBetweenSpawnedObjects = 50;
	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta=(ToolTip="Max height in which objects can spawn above center position",ClampMin="0",UIMin="0"), Category="EnvironmentAssistant|Zoning")
	int MaxSpawningHeight = 2000;
	UPROPERTY(BlueprintReadWrite,EditAnywhere , meta=(ToolTip="Min height in which objects can spawn below the center position", ClampMin="0",UIMin="0"), Category="EnvironmentAssistant|Zoning")
	int MinSpawningHeight = 2000;


	UPROPERTY(BlueprintReadWrite,EditAnywhere, meta=(ToolTip="If true objects will spawn within exclusion zone (Only at start)"), Category="EnvironmentAssistant")
	bool SpawnInsideExclusionOnStart = true;
protected:
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category="EnvironmentAssistant")
	bool AllowDebug;

private:
	FVector DirectionToSpawn;
	FVector* NewObjectSpawnLocation = new FVector;
	UPROPERTY(VisibleAnywhere, Category="EnvironmentAssistant")
	TArray<AEnvironmentObject*> SpawnedObjects; //Currently visible in world
	UPROPERTY(VisibleAnywhere, Category="EnvironmentAssistant")
	TArray<AEnvironmentObject*> InactiveObjects; //objects that are inactive and are used for object pooling 
	UPROPERTY()
	TArray<AEnvironmentObject*> ObjectsToDeSpawn; //Cached Objects to be despawned
	UPROPERTY()
	TArray<FString> SpawnableSurfaceNames;
};
