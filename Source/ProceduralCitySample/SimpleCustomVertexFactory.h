#pragma once
#include "CoreMinimal.h"
#include "VertexFactory.h"
#include "RenderResource.h"

// 继承 FVertexFactory
class PROCEDURALCITYSAMPLE_API FSimpleCustomVertexFactory : public FVertexFactory
{
    DECLARE_VERTEX_FACTORY_TYPE(FSimpleCustomVertexFactory);

public:
    FSimpleCustomVertexFactory(ERHIFeatureLevel::Type InFeatureLevel, const char* InDebugName)
        : FVertexFactory(InFeatureLevel)
    {}

    // 数据接口：用于 SceneProxy 传入 Buffer 指针
    struct FDataType
    {
        const FVertexBuffer* PositionComponent;   // Stream 0
        const FVertexBuffer* CustomDataComponent; // Stream 1
    };

    // 这一步把 Component 的 Buffer 指针缓存到 Render Thread 的 Factory 实例中
    void SetData(const FDataType& InData)
    {
        Data = InData;
        UpdateRHI(FRHICommandListExecutor::GetImmediateCommandList());
    }
    
    // 决定是否编译某个 Shader 变体
    static bool ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters);

    // 修改编译环境 (添加 #define)
    static void ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

    // --- RHI 资源初始化 ---
    virtual void InitRHI(FRHICommandListBase& RHICmdList) override;
    virtual void ReleaseRHI() override;

private:
    FDataType Data;
    FVertexDeclarationRHIRef VertexDeclarationRHI;
};