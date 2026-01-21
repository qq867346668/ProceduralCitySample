// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "NeonLightActor.generated.h"

class UStaticMeshComponent;
class UPointLightComponent;


UCLASS()
class PROCEDURALCITYSAMPLE_API ANeonLightActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANeonLightActor();

	UFUNCTION(BlueprintCallable)
	void TurnOn(bool turnOn);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	void NeonLightFlash();
	void SetNeonColorIntensity(float Scale);

private:
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* NeonMesh = nullptr;

	UPROPERTY(VisibleAnywhere)
	UPointLightComponent* NeonPointLight = nullptr;

	UPROPERTY(EditAnywhere)
	FLinearColor NeonColor = FLinearColor::Black;

	UPROPERTY(EditAnywhere)
	float NeonIntensity = 1.0;

	UPROPERTY(EditAnywhere)
	float FlashFrequence = 0.0;

	UPROPERTY(EditAnywhere)
	bool EnableRandomFrequence = 0.0;


	FTimerHandle TimerHandle;
	bool IsTurnOn = false;
};
