// Fill out your copyright notice in the Description page of Project Settings.


#include "MyProcedualActor.h"
#include "ProceduralMeshComponent.h"
#include "Async/Async.h"

// Sets default values
AMyProcedualActor::AMyProcedualActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CustomMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("CustomMesh"));
    RootComponent = CustomMesh;

    //CreateTriangle();
}

// Called when the game starts or when spawned
void AMyProcedualActor::BeginPlay()
{
	Super::BeginPlay();
	//CreateTriangle();
}

// Called every frame
void AMyProcedualActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyProcedualActor::RegenerateMeshAsync(int32 GridSize)
{
    const int32 CurrentJobID = ++TaskGenerationID;

    TWeakObjectPtr<AMyProcedualActor> WeakThis(this);

    TSharedPtr<FProcMeshData> NewData = MakeShareable(new FProcMeshData);
    UE::Tasks::TTask<void> MeshVertexTask = UE::Tasks::Launch(TEXT("AMyProcedualActor GeneratgeVertexData"), [WeakThis, GridSize, CurrentJobID, NewData]()
    {
        if (CurrentJobID != WeakThis->TaskGenerationID.load(std::memory_order_relaxed))
        {
            return; 
        }

        int32 GridCount = GridSize * GridSize;
        NewData->Vertices.Reserve(GridCount);
        NewData->Normals.Reserve(GridCount);
        for (int32 X = 0; X < GridSize; ++X)
        {
            for (int32 Y = 0; Y < GridSize; ++Y)
            {
                if (CurrentJobID != WeakThis->TaskGenerationID.load(std::memory_order_relaxed))
                {
                    return;
                }
                
                NewData->Vertices.Emplace(X * 10.0f, Y * 10.0f, 0);
                NewData->Normals.Emplace(0.0f, 0.0f, 1.0f);
            }
        }

        FPlatformProcess::Sleep(1.0f);

        float Z = CurrentJobID * 10.0f; 
        const int32 BatchCount = FMath::CeilToInt(GridCount / 64.0);
        ParallelFor(BatchCount, [&](int32 BatchIndex)
        {
            int32 MaxVertexIndex = 64;
            if ((BatchIndex + 1) * MaxVertexIndex  > GridCount)
            {
                MaxVertexIndex = MaxVertexIndex - (BatchIndex + 1) * MaxVertexIndex + GridCount;
            }

            for (int32 i = 0; i != MaxVertexIndex; ++i)
            {
                NewData->Vertices[i + BatchIndex * 64].Z = Z;
            }
        });

        FPlatformProcess::Sleep(1.0f);
    }, UE::Tasks::ETaskPriority::BackgroundHigh); 

    UE::Tasks::TTask<void> MeshIndexTask = UE::Tasks::Launch(TEXT("AMyProcedualActor GeneratgeIndices"), [WeakThis, GridSize, CurrentJobID, NewData]()
    {
        if (CurrentJobID != WeakThis->TaskGenerationID.load(std::memory_order_relaxed))
        {
            return; 
        }

        NewData->Indices.Reserve(GridSize * GridSize * 6);
        for (int32 Y = 0; Y < GridSize - 1; ++Y)
        {
            for (int32 X = 0; X < GridSize - 1; ++X)
            {
                if (CurrentJobID != WeakThis->TaskGenerationID.load(std::memory_order_relaxed))
                {
                    return;
                }

                const int32 BottomLeft = Y * GridSize + X;
                const int32 BottomRight = BottomLeft + 1;
                const int32 TopLeft = (Y + 1) * GridSize + X;
                const int32 TopRight = TopLeft + 1;

                // [Triangle 1] BottomLeft -> TopLeft -> TopRight (CCW)
                NewData->Indices.Add(BottomLeft);
                NewData->Indices.Add(TopRight);
                NewData->Indices.Add(TopLeft);

                // [Triangle 2] BottomLeft -> TopRight -> BottomRight (CCW)
                NewData->Indices.Add(BottomLeft);
                NewData->Indices.Add(BottomRight);
                NewData->Indices.Add(TopRight);
            }
        }

        FPlatformProcess::Sleep(2.0f);

        if (CurrentJobID != WeakThis->TaskGenerationID.load(std::memory_order_relaxed))
        {
            return; 
        }

        //Go back to GameThread
        // using MoveTemp to avoid large data copy
        AsyncTask(ENamedThreads::GameThread, [WeakThis, Result = NewData, CurrentJobID]() mutable
        {
            if (CurrentJobID != WeakThis->TaskGenerationID.load(std::memory_order_relaxed))
            {
                return; 
            }
            // [Game Thread] security check
            if (AMyProcedualActor* StrongThis = WeakThis.Get())
            {
                StrongThis->OnMeshGenerated(MoveTemp(*Result));
            }
            else
            {
                UE_LOG(LogTemp, Log, TEXT("zpfActor destroyed before task completion."));
            } 
        });
    }, MeshVertexTask, UE::Tasks::ETaskPriority::BackgroundHigh);
}

void AMyProcedualActor::OnMeshGenerated(FProcMeshData&& InData)
{
   CustomMesh->CreateMeshSection(0, InData.Vertices, InData.Indices, InData.Normals, TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), true);
   UE_LOG(LogTemp, Warning, TEXT("zpfCurrent Task ID:%d"), TaskGenerationID.load(std::memory_order_relaxed));
}

void AMyProcedualActor::CreateTriangle()
{
    TArray<FVector> Vertices;
    Vertices.Add(FVector(0, 0, 0));
    Vertices.Add(FVector(0, 100, 0));
    Vertices.Add(FVector(0, 0, 100));

    TArray<int32> Triangles;
    Triangles.Add(0);
    Triangles.Add(1);
    Triangles.Add(2);

    TArray<FVector> Normals;
    Normals.Add(FVector(-1, 0, 0));
    Normals.Add(FVector(-1, 0, 0));
    Normals.Add(FVector(-1, 0, 0));

    TArray<FVector2D> UV0;
    UV0.Add(FVector2D(0, 0));
    UV0.Add(FVector2D(1, 0));
    UV0.Add(FVector2D(0, 1));

    TArray<FLinearColor> VertexColors;
    VertexColors.Add(FLinearColor::Red);
    VertexColors.Add(FLinearColor::Green);
    VertexColors.Add(FLinearColor::Blue);

    TArray<FProcMeshTangent> Tangents;

    CustomMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, Normals, UV0, VertexColors, Tangents, true);
}

