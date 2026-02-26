// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/MeshComponent.h"
#include "MyMeshComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROCEDURALCITYSAMPLE_API UMyMeshComponent : public UMeshComponent
{
	GENERATED_BODY()

public:
    UMyMeshComponent();

    // [核心] 创建 SceneProxy，将数据从 GameThread 传递给 RenderThread
    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;

    // [核心] 计算包围盒，用于视锥剔除 (Frustum Culling)
    virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;	

    virtual int32 GetNumMaterials() const override;
    virtual UMaterialInterface* GetMaterial(int32 ElementIndex) const override;
};
