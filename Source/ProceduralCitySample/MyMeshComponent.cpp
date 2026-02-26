// Fill out your copyright notice in the Description page of Project Settings.


#include "MyMeshComponent.h"

//RenderThread Resource Definition
#include "PrimitiveSceneProxy.h"
#include "SimpleCustomVertexFactory.h"
#include "DynamicMeshBuilder.h"
#include "MaterialDomain.h"
#include "Materials/MaterialRenderProxy.h"
#include "PrimitiveUniformShaderParametersBuilder.h"

#define USE_CUSTOM_VERTEX_FACTORY 

class FMyTriangleSceneProxy : public FPrimitiveSceneProxy
{
public:
    SIZE_T GetTypeHash() const override
    {
        static size_t UniquePointer;
        return reinterpret_cast<size_t>(&UniquePointer);
    }

    FMyTriangleSceneProxy(UMyMeshComponent* Component)
        : FPrimitiveSceneProxy(Component)
        , MaterialRelevance(Component->GetMaterialRelevance(GetScene().GetFeatureLevel()))
        , VertexFactory(GetScene().GetFeatureLevel(), "FMyTriangleSceneProxy")
    {
        // 1. Get Material
        MaterialInterface = Component->GetMaterial(0);
        if (!MaterialInterface)
        {
            MaterialInterface = UMaterial::GetDefaultMaterial(MD_Surface);
        }

        CreateTriangle();
    }

    virtual ~FMyTriangleSceneProxy()
    {
#ifdef USE_CUSTOM_VERTEX_FACTORY
        PositionBuffer.ReleaseResource();
        CustomDataBuffer.ReleaseResource();
#else
        VertexBuffers.PositionVertexBuffer.ReleaseResource();
        VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
        VertexBuffers.ColorVertexBuffer.ReleaseResource();
#endif

        IndexBuffer.ReleaseResource();
        VertexFactory.ReleaseResource();
    }

    void CreateTriangle()
    {
#ifdef USE_CUSTOM_VERTEX_FACTORY
        TArray<FVector3f> Positions = { 
            FVector3f(0,0,0), FVector3f(0,100,0), FVector3f(0,0,100) 
        };
        
        TArray<FVector3f> CustomData = {
            FVector4f(1,0,0), 
            FVector4f(0,1,0), 
            FVector4f(0,0,1)
        };
        TArray<uint32> Indices = { 0, 1, 2 };

        PositionBuffer.Init(Positions);
        CustomDataBuffer.Init(CustomData);
        IndexBuffer.Indices = Indices;

        FSimpleCustomVertexFactory::FDataType Data;
        Data.PositionComponent = &PositionBuffer;
        Data.CustomDataComponent = &CustomDataBuffer;
        VertexFactory.SetData(Data);

        ENQUEUE_RENDER_COMMAND(MyTriangleSceneProxyResourceInit)(
			[this](FRHICommandListImmediate& RHICmdList)
			{
				PositionBuffer.InitResource(RHICmdList);
				CustomDataBuffer.InitResource(RHICmdList);
				VertexFactory.InitResource(RHICmdList);
				IndexBuffer.InitResource(RHICmdList);
			});
#else
        FVector3f NormalDir = FVector3f(-1.0f, 0.0f, 0.0f);
        FVector3f TangentDir = FVector3f(0.0f, 1.0f, 0.0f);

        TArray<FDynamicMeshVertex> Vertices;
        Vertices.Add(FDynamicMeshVertex(FVector3f(0, 0, 0), TangentDir, NormalDir, FVector2f(0, 0), FColor::Red));
        Vertices.Add(FDynamicMeshVertex(FVector3f(0, 100, 0), TangentDir, NormalDir, FVector2f(0, 1), FColor::Green));
        Vertices.Add(FDynamicMeshVertex(FVector3f(0, 0, 100), TangentDir, NormalDir, FVector2f(1, 0), FColor::Blue));

        TArray<uint32> Indices = { 0, 1, 2 };

        VertexBuffers.InitFromDynamicVertex(&VertexFactory, Vertices);
        IndexBuffer.Indices = Indices;

        ENQUEUE_RENDER_COMMAND(MyTriangleSceneProxyResourceInit)(
			[this](FRHICommandListImmediate& RHICmdList)
			{
				VertexBuffers.PositionVertexBuffer.InitResource(RHICmdList);
				VertexBuffers.StaticMeshVertexBuffer.InitResource(RHICmdList);
				VertexBuffers.ColorVertexBuffer.InitResource(RHICmdList);
				VertexFactory.InitResource(RHICmdList);
				IndexBuffer.InitResource(RHICmdList);
			});
#endif
    }
    

    virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
    {
        QUICK_SCOPE_CYCLE_COUNTER(STAT_MyTriangleSceneProxy_GetDynamicMeshElements);

        const bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;
        auto WireframeMaterialInstance = new FColoredMaterialRenderProxy(
			GEngine->WireframeMaterial ? GEngine->WireframeMaterial->GetRenderProxy() : NULL,
			FLinearColor(0, 0.5f, 1.f)
			);

		Collector.RegisterOneFrameMaterialProxy(WireframeMaterialInstance);
        
        FMaterialRenderProxy* MaterialProxy = bWireframe ? WireframeMaterialInstance : MaterialInterface->GetRenderProxy();

        for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
        {
            if (VisibilityMap & (1 << ViewIndex))
            {
                FMeshBatch& Mesh = Collector.AllocateMesh();
                Mesh.bWireframe = bWireframe;
                Mesh.MaterialRenderProxy = MaterialProxy;
                Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
                Mesh.Type = PT_TriangleList;
                Mesh.DepthPriorityGroup = SDPG_World;
                Mesh.bCanApplyViewModeOverrides = false;

                Mesh.VertexFactory = &VertexFactory;
                
                FMeshBatchElement& BatchElement = Mesh.Elements[0];
                BatchElement.IndexBuffer = &IndexBuffer;
                BatchElement.FirstIndex = 0;
                BatchElement.NumPrimitives = 1; 
                BatchElement.MinVertexIndex = 0;
                BatchElement.MaxVertexIndex = 2;
                
                // FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
                // DynamicPrimitiveUniformBuffer.Set(Collector.GetRHICommandList(), GetLocalToWorld(), GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, false, AlwaysHasVelocity());
                // BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;

                // bool bHasPrecomputedVolumetricLightmap;
                // FMatrix PreviousLocalToWorld;
                // int32 SingleCaptureIndex;
                // bool bOutputVelocity;
                // GetScene().GetPrimitiveUniformShaderParameters_RenderThread(
                //     GetPrimitiveSceneInfo(), 
                //     bHasPrecomputedVolumetricLightmap, 
                //     PreviousLocalToWorld, // 这里会填入正确的上一帧矩阵
                //     SingleCaptureIndex, 
                //     bOutputVelocity
                // );

                // FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
                //     DynamicPrimitiveUniformBuffer.Set(
                //     Collector.GetRHICommandList(), 
                //     GetLocalToWorld(), 
                //     PreviousLocalToWorld, // <--- 使用获取到的 Previous 矩阵，而不是 GetLocalToWorld()
                //     GetBounds(), 
                //     GetLocalBounds(), 
                //     true, 
                //     bHasPrecomputedVolumetricLightmap, 
                //     bOutputVelocity       // <--- 使用系统计算的 Velocity 标记
                // );

                // BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;

                //BatchElement.PrimitiveUniformBuffer = GetUniformBuffer();

#ifndef USE_CUSTOM_VERTEX_FACTORY
                FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
				FPrimitiveUniformShaderParametersBuilder Builder;
				BuildUniformShaderParameters(Builder);
				DynamicPrimitiveUniformBuffer.Set(Collector.GetRHICommandList(), Builder);
                BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;
#else
                BatchElement.PrimitiveUniformBuffer = GetUniformBuffer();
#endif

                Collector.AddMesh(ViewIndex, Mesh);
            }
        }
    }

    virtual void DrawStaticElements(FStaticPrimitiveDrawInterface* PDI) override
    {
        FMeshBatch Mesh;
        Mesh.bWireframe = false;
        Mesh.MaterialRenderProxy = MaterialInterface->GetRenderProxy();
        Mesh.Type = PT_TriangleList;
        Mesh.VertexFactory = &VertexFactory;
        Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
        Mesh.DepthPriorityGroup = SDPG_World;
        //Must set, Gemini forget this
        Mesh.LODIndex = 0;
		//Mesh.SegmentIndex = 0;
    
        FMeshBatchElement& BatchElement = Mesh.Elements[0];
        BatchElement.IndexBuffer = &IndexBuffer;
        BatchElement.FirstIndex = 0;
        BatchElement.NumPrimitives = 1;
        BatchElement.MinVertexIndex = 0;
        BatchElement.MaxVertexIndex = 2;
        BatchElement.PrimitiveUniformBuffer = GetUniformBuffer();
    
        PDI->DrawMesh(Mesh, 1.0f);
    }


    virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
    {
        FPrimitiveViewRelevance Result;
        Result.bDrawRelevance = IsShown(View);
        Result.bShadowRelevance = IsShadowCast(View);
        //Result.bDynamicRelevance = true; // declare as dynamic object，will invoke GetDynamicMeshElements
        Result.bStaticRelevance = true;
        Result.bRenderInMainPass = ShouldRenderInMainPass();
        Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
        Result.bRenderCustomDepth = ShouldRenderCustomDepth();
        
        MaterialRelevance.SetPrimitiveViewRelevance(Result);
        //Must set, Gemini forget this
        Result.bVelocityRelevance = DrawsVelocity() && Result.bOpaque && Result.bRenderInMainPass;
        return Result;
    }

    virtual uint32 GetMemoryFootprint() const override { return sizeof(*this) + GetAllocatedSize(); }

private:
    UMaterialInterface* MaterialInterface;
    FMaterialRelevance MaterialRelevance;
    
    FDynamicMeshIndexBuffer32 IndexBuffer;

#ifdef USE_CUSTOM_VERTEX_FACTORY
    FSimpleCustomVertexFactory VertexFactory;
    FPositionVertexBuffer PositionBuffer;
    FPositionVertexBuffer CustomDataBuffer;
#else
    FLocalVertexFactory VertexFactory;
    FStaticMeshVertexBuffers VertexBuffers; 
#endif
};

//UMyMeshComponent Definition
UMyMeshComponent::UMyMeshComponent()
{
    Bounds = FBoxSphereBounds(FBox(FVector(0, 0, 0), FVector(100, 100, 100)));
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UMyMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    //MarkRenderStateDirty();
    //MarkRenderTransformDirty();
}

FPrimitiveSceneProxy* UMyMeshComponent::CreateSceneProxy()
{
    return new FMyTriangleSceneProxy(this);
}

FBoxSphereBounds UMyMeshComponent::CalcBounds(const FTransform& LocalToWorld) const
{
    return FBoxSphereBounds(FBox(FVector(0, 0, 0), FVector(100, 100, 100))).TransformBy(LocalToWorld);
}

int32 UMyMeshComponent::GetNumMaterials() const 
{
    return 1;
}

UMaterialInterface* UMyMeshComponent::GetMaterial(int32 ElementIndex) const 
{
    if (ElementIndex != 0) 
    {
        return nullptr;
    }

    if (OverrideMaterials.IsValidIndex(ElementIndex) && OverrideMaterials[ElementIndex])
    {
        return OverrideMaterials[ElementIndex];
    }

    return nullptr;
}