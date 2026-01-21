// Fill out your copyright notice in the Description page of Project Settings.


#include "CleanupDuplicatedBuildings.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACleanupDuplicatedBuildings::ACleanupDuplicatedBuildings()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACleanupDuplicatedBuildings::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACleanupDuplicatedBuildings::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACleanupDuplicatedBuildings::RemoveDuplicateActors()
{
	static const float Tolerance = 100.0f;
	
	if (!GetWorld()) return;
    
    TArray<AActor*> ActorsWithTag;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), TEXT("PCG Generated Actor"), ActorsWithTag);
    
    if (ActorsWithTag.Num() == 0) return;
    
    TArray<FVector> KeptLocations;
    TArray<AActor*> ActorsToDestroy;
    
    for (AActor* Actor : ActorsWithTag)
    {
        if (!IsValid(Actor)) continue;
        
        FVector ActorLocation = Actor->GetActorLocation();
        bool bIsDuplicate = false;
     
        for (const FVector& KeptLocation : KeptLocations)
        {
            if (FVector::Dist(ActorLocation, KeptLocation) <= Tolerance)
            {
                bIsDuplicate = true;
                break;
            }
        }
        
        if (bIsDuplicate)
        {
            ActorsToDestroy.Add(Actor);
        }
        else
        {
            KeptLocations.Add(ActorLocation);
        }
    }
    
    for (AActor* ActorToDestroy : ActorsToDestroy)
    {
        if (IsValid(ActorToDestroy))
        {
            ActorToDestroy->Destroy();
        }
    }
    
    if (ActorsToDestroy.Num() > 0 && GEngine)
    {
        GEngine->ForceGarbageCollection(true);
    }
}

