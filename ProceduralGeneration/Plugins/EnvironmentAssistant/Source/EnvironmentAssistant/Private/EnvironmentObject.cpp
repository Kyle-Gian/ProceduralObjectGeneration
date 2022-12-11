// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvironmentObject.h"

// Sets default values
AEnvironmentObject::AEnvironmentObject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("ObjectMesh",false);

}

void AEnvironmentObject::Initialize(UStaticMesh* mesh, float offset)
{
	StaticMesh->SetStaticMesh(mesh);
	HeightOffset = offset;
	StaticMesh->SetRelativeLocation(StaticMesh->GetRelativeLocation() + HeightOffset);
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

