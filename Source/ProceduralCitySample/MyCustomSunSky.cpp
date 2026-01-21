// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCustomSunSky.h"

// Sets default values
AMyCustomSunSky::AMyCustomSunSky()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AMyCustomSunSky::BeginPlay()
{
	if (!ViewExt.IsValid())
	{
		ViewExt = FSceneViewExtensions::NewExtension<FMyViewExtension>(TWeakObjectPtr<AMyCustomSunSky>(this));
	}

	Super::BeginPlay();
}

void AMyCustomSunSky::OnConstruction(const FTransform& Transform)
{
	if (!ViewExt.IsValid())
	{
		ViewExt = FSceneViewExtensions::NewExtension<FMyViewExtension>(TWeakObjectPtr<AMyCustomSunSky>(this));
	}

	Super::OnConstruction(Transform);
}

void AMyCustomSunSky::BeginDestroy()
{
	//UE_LOG(LogTemp, Warning, TEXT("AMyCustomSunSky::BeginDestroy............."));
	ViewExt.Reset();
	Super::BeginDestroy();
}

FMyViewExtension::FMyViewExtension(const FAutoRegister& AutoRegister, TWeakObjectPtr<AMyCustomSunSky> InOwner)
: ISceneViewExtension(), Owner(InOwner) 
{

}

bool FMyViewExtension::IsActiveThisFrame(const FSceneViewExtensionContext& Context) const
{
	return Owner.IsValid() && Context.GetWorld() == Owner->GetWorld();
}

void FMyViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
	//UE_LOG(LogTemp, Warning, TEXT("FMyViewExtension::SetupViewFamily............."));
	if (InViewFamily.Views.Num() == 1 &&
		InViewFamily.Views[0]->bIsReflectionCapture &&
		!InViewFamily.EngineShowFlags.SkyLighting) {
		//UE_LOG(LogTemp, Warning, TEXT("FMyViewExtension::RecaptureSkylightING............."));
		InViewFamily.EngineShowFlags.VolumetricFog = false;
	}
}

