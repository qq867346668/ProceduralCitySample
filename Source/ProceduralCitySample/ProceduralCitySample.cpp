// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralCitySample.h"
#include "Modules/ModuleManager.h"
#include "ShaderCore.h" // must include
#include "Misc/Paths.h"

class FMyProjectModule : public FDefaultGameModuleImpl
{
public:
    virtual void StartupModule() override
    {
        FString ShaderDirectory = FPaths::Combine(FPaths::ProjectDir(), TEXT("Shaders"));
        AddShaderSourceDirectoryMapping(TEXT("/Project/MyShaders"), ShaderDirectory);
    }
};s

IMPLEMENT_PRIMARY_GAME_MODULE( FMyProjectModule, ProceduralCitySample, "ProceduralCitySample" );
 