// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CleanupDuplicatedBuildings.generated.h"

UCLASS()
class PROCEDURALCITYSAMPLE_API ACleanupDuplicatedBuildings : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACleanupDuplicatedBuildings();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor, Category = "Actor Management")
	void RemoveDuplicateActors();
};
