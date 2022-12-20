// Copyright 2022, Kyle Gian, All rights reserved


#include "EnvironmentObject.h"

// Sets default values
AEnvironmentObject::AEnvironmentObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
    if (!StaticMesh)
    	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("ObjectMesh",false);

}

void AEnvironmentObject::Initialize(UStaticMesh* mesh, float offset)
{
	if (mesh != NULL)
		StaticMesh->SetStaticMesh(mesh);

	HeightOffset = offset;
	StaticMesh->SetRelativeLocation(StaticMesh->GetRelativeLocation() + HeightOffset);
}

void AEnvironmentObject::MoveObject(FVector newLocation)
{
	newLocation.Z += HeightOffset;
	SetActorLocation(newLocation);
}

void AEnvironmentObject::HideObject()
{
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
}

void AEnvironmentObject::UnHideObject()
{
	SetActorEnableCollision(true);
	SetActorHiddenInGame(false);
}

// Called when the game starts or when spawned
void AEnvironmentObject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnvironmentObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

