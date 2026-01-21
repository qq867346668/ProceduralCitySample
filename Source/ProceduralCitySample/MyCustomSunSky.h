// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SceneViewExtension.h" 
#include "MyCustomSunSky.generated.h"

UCLASS()
class PROCEDURALCITYSAMPLE_API AMyCustomSunSky : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyCustomSunSky();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginDestroy() override;

private:
	TSharedPtr<class FMyViewExtension, ESPMode::ThreadSafe> ViewExt;
};

class FMyViewExtension : public ISceneViewExtension
{
public:
	FMyViewExtension(const FAutoRegister& AutoRegister, TWeakObjectPtr<AMyCustomSunSky> InOwner);

	// 只在 Actor 所在的世界里启用，避免编辑器/其他 PIE 窗口也触发
	virtual bool IsActiveThisFrame(const FSceneViewExtensionContext& Context) const override;
	
	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override {}
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override {}

private:
	TWeakObjectPtr<AMyCustomSunSky> Owner;
};
