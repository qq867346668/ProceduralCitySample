// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyProcedualActor.generated.h"

class UProceduralMeshComponent;

struct FProcMeshData
{
    TArray<FVector> Vertices;
    TArray<int32> Indices;
    TArray<FVector> Normals;
};

UCLASS(Blueprintable)
class PROCEDURALCITYSAMPLE_API AMyProcedualActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyProcedualActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Async")
    void RegenerateMeshAsync(int32 GridSize);

	UPROPERTY(BluePrintReadOnly, VisibleAnywhere)
	TObjectPtr<UProceduralMeshComponent> CustomMesh;

private:
	void CreateTriangle();

private:
	void OnMeshGenerated(FProcMeshData&& InData);

	std::atomic<int32> TaskGenerationID = {0};
};
