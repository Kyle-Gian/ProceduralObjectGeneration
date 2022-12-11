// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnvironmentObject.generated.h"

UCLASS()
class ENVIRONMENTASSISTANT_API AEnvironmentObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvironmentObject();
	void Initialize(UStaticMesh* mesh, float offset);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

	void SetMesh(UStaticMeshComponent* newMesh) { StaticMesh = newMesh;}
	UStaticMeshComponent* GetMesh() const {return StaticMesh;}
	float HeightOffset;

protected:
	UPROPERTY()
	UStaticMeshComponent* StaticMesh;
};
