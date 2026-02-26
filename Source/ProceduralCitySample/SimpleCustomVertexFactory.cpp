#include "SimpleCustomVertexFactory.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"
#include "DataDrivenShaderPlatformInfo.h"
#include "MeshMaterialShader.h"
#include "MaterialDomain.h"

bool FSimpleCustomVertexFactory::ShouldCompilePermutation(const FVertexFactoryShaderPermutationParameters& Parameters)
{
    // 只允许在 Surface 材质域使用，且排除 UI 等特殊域
    return Parameters.MaterialParameters.MaterialDomain == MD_Surface;
}

void FSimpleCustomVertexFactory::ModifyCompilationEnvironment(const FVertexFactoryShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment)
{
   OutEnvironment.SetDefine(TEXT("RAY_TRACING_DYNAMIC_MESH_IN_LOCAL_SPACE"), TEXT("1"));
}

void FSimpleCustomVertexFactory::InitRHI(FRHICommandListBase& RHICmdList)
{
    FVertexDeclarationElementList Elements;
	Elements.Add(AccessStreamComponent(FVertexStreamComponent(Data.PositionComponent, 0, sizeof(FVector3f), VET_Float3), 0));
	Elements.Add(AccessStreamComponent(FVertexStreamComponent(Data.CustomDataComponent, 0, sizeof(FVector3f), VET_Float3), 1));
	InitDeclaration(Elements);
}

void FSimpleCustomVertexFactory::ReleaseRHI()
{   
    VertexDeclarationRHI.SafeRelease();
}

IMPLEMENT_VERTEX_FACTORY_TYPE(FSimpleCustomVertexFactory, "/Project/MyShaders/SimpleCustomVertexFactory.ush", 
      EVertexFactoryFlags::UsedWithMaterials
	| EVertexFactoryFlags::SupportsStaticLighting
	| EVertexFactoryFlags::SupportsDynamicLighting
	| EVertexFactoryFlags::SupportsPrecisePrevWorldPos
	| EVertexFactoryFlags::SupportsCachingMeshDrawCommands
	| EVertexFactoryFlags::SupportsLightmapBaking
	| EVertexFactoryFlags::SupportsPSOPrecaching);