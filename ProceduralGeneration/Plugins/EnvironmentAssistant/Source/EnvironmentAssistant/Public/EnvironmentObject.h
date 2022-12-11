// Copyright 2022, Kyle Gian, All rights reserved


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "EnvironmentObject.generated.h"

UCLASS()
class ENVIRONMENTASSISTANT_API AEnvironmentObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnvironmentObject();
	void Initialize(UStaticMesh* mesh, float offset);
	UFUNCTION(Category="EnvironmentObject")
	void MoveObject(FVector newLocation);
	
	UFUNCTION(BlueprintCallable, Category="EnvironmentObject")
	virtual void HideObject();
	UFUNCTION(BlueprintCallable, Category="EnvironmentObject")
	virtual void UnHideObject();
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
	UPROPERTY(BlueprintReadOnly, Category="EnvironmentObject")
	UStaticMeshComponent* StaticMesh;
};
