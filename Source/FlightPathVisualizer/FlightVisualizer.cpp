// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightVisualizer.h"
#include "FlightDataManager.h"
#include "DrawDebugHelpers.h"
#include "FlightMathLibrary.h"
#include "FlightPathSplineActor.h"
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

    if (WorldScale != 1.0f)
    {
        for (FVector& P : LocalPoints)
        {
            P *= WorldScale;
        }
    }

    UE_LOG(LogTemp, Log, TEXT("[Visualizer] Converted %d points to ENU space."), LocalPoints.Num());

    /*AFlightPathSplineActor* SplineActor = GetWorld()->SpawnActor<AFlightPathSplineActor>();
    SplineActor->BuildSplineFromPoints(LocalPoints);

    if (SplineMesh)
    {
        SplineActor->BuildSplineMeshes(SplineMesh, 10.0f);
	}
    else
    {
		UE_LOG(LogTemp, Warning, TEXT("[Visualizer] SplineMesh is NULL, cannot build spline meshes."));
    }*/

    CaculateFlightStats(GPSPoints);
    
    FlushPersistentDebugLines(GetWorld());

    //float DebugScale = 50.0f;

    //if (LocalPoints.Num() > 1)
    //{
    //    for (int32 i = 0; i < LocalPoints.Num() - 1; i++)
    //    {
    //        DrawDebugLine(
    //            GetWorld(),
    //            LocalPoints[i] * DebugScale,         // Điểm bắt đầu
    //            LocalPoints[i + 1] * DebugScale,     // Điểm kết thúc
    //            FColor::Green,           // Màu sắc (Cyan cho dễ nhìn trên nền tối)
    //            true,                   // Persistent = true (giữ nguyên trên màn hình không biến mất)
    //            -1.0f,                  // Lifetime (vô hạn nếu Persistent=true)
    //            0,                      // DepthPriority
    //            10.0f                    // Thickness (độ dày đường)
    //        );
    //    }
    //}

    AFlightPathSplineActor* SplineActor = GetWorld()->SpawnActor<AFlightPathSplineActor>();

    // 1. Cấu hình Mesh cho Waypoint (các điểm)
    if (WaypointMesh)
    {
        SplineActor->WaypointMesh = WaypointMesh;
    }
    else
    {
        // Fallback: Nếu chưa chọn WaypointMesh, dùng tạm SplineMesh (nếu có)
        if (SplineMesh) SplineActor->WaypointMesh = SplineMesh;
    }

   
    SplineActor->WaypointScale = WaypointScale;

    // Chọn sửa lỗi phù hợp dựa trên kiểu thực tế của SplineActor->WaypointScale

    if (SplineMaterial)
    {
        SplineActor->SplineMaterial = SplineMaterial;
    }

    // 2. Tạo dữ liệu Spline và vẽ các Waypoint (ISMC)
    SplineActor->BuildSplineFromPoints(LocalPoints);

    // 3. [QUAN TRỌNG] Tạo Mesh cho đường nối (Spline Mesh)
    if (SplineMesh)
    {
        // Width = 20.0f (hoặc số tùy ý), chỉnh độ dày đường nối
        SplineActor->BuildSplineMeshes(SplineMesh, Thickness);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[Visualizer] Chưa chọn SplineMesh! Chỉ hiển thị Waypoint."));
    }

    UE_LOG(LogTemp, Log, TEXT("[Visualizer] Spawned SplineActor and built path."));


    
    //DrawDebugPoints(LocalPoints);
}

void AFlightVisualizer::CaculateFlightStats(const TArray<FFlightPoint>& GPSPoints)
{
    if (GPSPoints.Num() < 2) {
        return;
    }

    double TotalDistMeter = 0.0;
    double TotalAlt = 0.0;

    for (int32 i = 0; i < GPSPoints.Num(); i++)
    {
		TotalAlt += GPSPoints[i].Altitude;
        UE_LOG(LogTemp, Log, TEXT("Point %d: Lat=%f, Lon=%f, Alt=%f, Timestamp=%f"), 
			i, GPSPoints[i].Latitude, GPSPoints[i].Longitude, GPSPoints[i].Altitude, GPSPoints[i].TimeInSeconds);
        if (i > 0) 
        {
            //c1: su dung cong thuc haversine de tinh toan khoang cach giua 2 diem tren GPS
            //TotalDistMeter += UFlightMathLibary::HaversineDistance(GPSPoints[i - 1], GPSPoints[i]);

            //c2: su dung khoang cach cua euclidean trong unreal
            if (CoordinateSystem)
            {
                FVector P1 = CoordinateSystem->ConvertSingle(GPSPoints[i - 1]);
                FVector P2 = CoordinateSystem->ConvertSingle(GPSPoints[i]);
				TotalDistMeter += FVector::Dist(P1, P2);
            }
        }
    }

    //tong khoang cach duoc tinh bang km
	TotalDistanceKm = (float)(TotalDistMeter / 1000.0);

    //tong do cao trung binh bay 
	AvgAltitudeMeters = (float)(TotalAlt / GPSPoints.Num());

    TotalFlightTimeSec = (float)(GPSPoints.Last().TimeInSeconds - GPSPoints[0].TimeInSeconds);

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
            10,     // sphere radius
            12,
            FColor::Green,
            true,
            600
        );

        UE_LOG(LogTemp, Log, TEXT("Scaled Debug Point %d = %s"),
            i, *ScaledPos.ToString());
    }
}

FFlightStats AFlightVisualizer::GetFlightStatistics() const
{
    FFlightStats Stats;
    Stats.TotalDistanceKm = TotalDistanceKm;
    Stats.TotalFlightTimeSec = TotalFlightTimeSec;
    Stats.AvgAltitudeMeters = AvgAltitudeMeters;
    return Stats;
}

