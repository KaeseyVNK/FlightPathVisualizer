// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightVisualizer.h"
#include "FlightDataManager.h"
#include "DrawDebugHelpers.h"
// Sets default values
AFlightVisualizer::AFlightVisualizer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFlightVisualizer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlightVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFlightVisualizer::LoadAndVisualizeFlightPath(const FString& CSVPath)
{
   /* if (!GeoConverter)
    {
        UE_LOG(LogTemp, Error, TEXT("[Visualizer] GeoConverter is NULL! Assign it in the Editor."));
        return;
    }

    
    TArray<FFlightPoint> GPSPoints;
    bool bParsed = UFlightDataManager::ParseCSV(CSVPath, GPSPoints);

    if (!bParsed || GPSPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[Visualizer] Failed to parse CSV: %s"), *CSVPath);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[Visualizer] Parsed %d GPS points."), GPSPoints.Num());

   
    TArray<FVector> LocalPoints;
    bool bConverted = GeoConverter->ConvertGPSArrayToUE(GPSPoints, LocalPoints);

    if (!bConverted)
    {
        UE_LOG(LogTemp, Error, TEXT("[Visualizer] GPS conversion failed!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[Visualizer] Conversion OK. Debug spheres spawned for %d points."), LocalPoints.Num());*/

	//============-=====================-===========================
   /* if (!GeoConverter)
    {
        UE_LOG(LogTemp, Error, TEXT("[Visualizer] GeoConverter is NULL! Assign it in the Editor."));
        return;
    }

    TArray<FFlightPoint> GPSPoints;
    bool bParsed = UFlightDataManager::ParseCSV(CSVPath, GPSPoints);

    if (!bParsed || GPSPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[Visualizer] Failed to parse CSV: %s"), *CSVPath);
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[Visualizer] Parsed %d GPS points."), GPSPoints.Num());

    
    GeoConverter->SetOriginFromFirstPoint(GPSPoints[0]);

    TArray<FVector> LocalPoints;
    bool bConverted = GeoConverter->ConvertGPSArrayToUE(GPSPoints, LocalPoints);

    if (!bConverted)
    {
        UE_LOG(LogTemp, Error, TEXT("[Visualizer] GPS conversion failed!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[Visualizer] Conversion OK. Debug spheres spawned for %d points."), LocalPoints.Num());*/
	//===========================================

    UE_LOG(LogTemp, Log, TEXT("[Visualizer] Selected file: %s"), *CSVPath);

    TArray<FFlightPoint> GPSPoints;
    if (!UFlightDataManager::ParseCSV(CSVPath, GPSPoints) || GPSPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[Visualizer] Failed to parse CSV!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("[Visualizer] Parsed %d GPS points."), GPSPoints.Num());

  
    if (!CoordinateSystem)
    {
        CoordinateSystem = GetWorld()->SpawnActor<AFlightCoordinateActor>();
        UE_LOG(LogTemp, Warning, TEXT("[Visualizer] CoordinateSystem was NULL Spawned new one."));
    }

    
    CoordinateSystem->InitializeFromPoint(GPSPoints[0]);

   
    TArray<FVector> LocalPoints;
    CoordinateSystem->ConvertArray(GPSPoints, LocalPoints);

    UE_LOG(LogTemp, Log, TEXT("[Visualizer] Converted %d points to ENU space."), LocalPoints.Num());

    
    DrawDebugPoints(LocalPoints);
}

void AFlightVisualizer::DrawDebugPoints(const TArray<FVector>& Points)
{
    UWorld* World = GetWorld();
    if (!World) return;

    float DebugScale = 50.0f; // phóng to 50 lần

    for (int32 i = 0; i < Points.Num(); i++)
    {
        FVector ScaledPos = Points[i] * DebugScale;

        DrawDebugSphere(
            GetWorld(),
            ScaledPos,
            30,     // sphere radius
            12,
            FColor::Red,
            false,
            600
        );

        UE_LOG(LogTemp, Log, TEXT("Scaled Debug Point %d = %s"),
            i, *ScaledPos.ToString());
    }
}

