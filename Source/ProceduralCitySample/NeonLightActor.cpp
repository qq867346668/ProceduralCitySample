// Fill out your copyright notice in the Description page of Project Settings.


#include "NeonLightActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"

// Sets default values
ANeonLightActor::ANeonLightActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	NeonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NeonMesh"));
	NeonMesh->SetupAttachment(RootComponent);

	// NeonPointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("NeonLight"));
	// NeonPointLight->SetupAttachment(NeonMesh);
}

void ANeonLightActor::TurnOn(bool turnOn)
{
	IsTurnOn = turnOn;
	if (!IsTurnOn)
	{
		SetNeonColorIntensity(0.01);
	}
}

// Called when the game starts or when spawned
void ANeonLightActor::BeginPlay()
{
	Super::BeginPlay();

	NeonMesh->SetScalarParameterValueOnMaterials(TEXT("EmissiveScale"), NeonIntensity);
	float frequence = FlashFrequence;
	if (EnableRandomFrequence)
	{
		FlashFrequence *= FMath::FRand();
	}

	if (FlashFrequence > 0)
	{
		FTimerManager& TimerManager = GetWorld()->GetTimerManager();
		TimerManager.SetTimer(TimerHandle, this, &ANeonLightActor::NeonLightFlash, FlashFrequence, true);
	}

	SetNeonColorIntensity(1.0);
}

void ANeonLightActor::NeonLightFlash()
{
	if (!IsTurnOn)
	{
		return;
	}

	static bool FLASHON = true;
	SetNeonColorIntensity(FLASHON);

	FLASHON = !FLASHON;
}

void ANeonLightActor::SetNeonColorIntensity(float Scale)
{
	FLinearColor clr = NeonColor * Scale;
	NeonMesh->SetVectorParameterValueOnMaterials(TEXT("Color"), FVector(clr));
	// NeonPointLight->SetLightColor(clr);
}

