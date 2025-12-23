// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightVisualizer.h"
#include "FlightDataManager.h"
#include "DrawDebugHelpers.h"
#include "FlightMathLibrary.h" // Nhớ include thư viện toán
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
    // === 1. DỌN DẸP DỮ LIỆU CŨ ===
    // Nếu đã có SplineActor từ lần load trước, hãy hủy nó đi
    if (CurrentSplineActor)
    {
        CurrentSplineActor->Destroy();
        CurrentSplineActor = nullptr;
    }

    // Xóa dữ liệu mảng cũ
    ParsedGPSPoints.Empty();
    // =============================

    UE_LOG(LogTemp, Log, TEXT("[Visualizer] Selected file: %s"), *CSVPath);

    TArray<FFlightPoint> GPSPoints;
    if (!UFlightDataManager::ParseCSV(CSVPath, GPSPoints) || GPSPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Error, TEXT("[Visualizer] Failed to parse CSV!"));
        return;
    }

    ParsedGPSPoints = GPSPoints;

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

    //UE_LOG(LogTemp, Log, TEXT("[Visualizer] Converted %d points to NED space."), LocalPoints.Num());
    // UE_LOG(LogTemp, Log, TEXT("[Visualizer] Converted %d points to ENU space."), LocalPoints.Num());
    UE_LOG(LogTemp, Log, TEXT("[Visualizer] Converted %d points to NED->Unreal space."), LocalPoints.Num());

    // [TEST] Kiểm tra độ chính xác của Dead Reckoning
    CoordinateSystem->TestDeadReckoningAccuracy(GPSPoints);

    // [TEST] Kiểm tra độ chính xác của Dead Reckoning
    CoordinateSystem->TestDeadReckoningAccuracy(GPSPoints);

    // [TEST] Kiểm tra công thức WGS84
    CoordinateSystem->TestWGS84Accuracy();

    CaculateFlightStats(GPSPoints);
    
    FlushPersistentDebugLines(GetWorld());

    // === 2. TẠO MỚI SPLINE ACTOR VÀ LƯU VÀO BIẾN CurrentSplineActor ===
    CurrentSplineActor = GetWorld()->SpawnActor<AFlightPathSplineActor>();

    // Cấu hình Mesh cho Waypoint (các điểm)
    if (WaypointMesh)
    {
        CurrentSplineActor->WaypointMesh = WaypointMesh;
    }
    else
    {
        if (SplineMesh) CurrentSplineActor->WaypointMesh = SplineMesh;
    }

    CurrentSplineActor->WaypointScale = WaypointScale;

    if (SplineMaterial)
    {
        CurrentSplineActor->SplineMaterial = SplineMaterial;
    }

    // Tạo dữ liệu Spline và vẽ các Waypoint (ISMC)
    CurrentSplineActor->BuildSplineFromPoints(LocalPoints);

    // Tạo Mesh cho đường nối (Spline Mesh)
    if (SplineMesh)
    {
        CurrentSplineActor->BuildSplineMeshes(SplineMesh, Thickness);
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
			i, GPSPoints[i].Latitude, GPSPoints[i].Longitude, GPSPoints[i].Altitude, GPSPoints[i].TimeInSeconds );
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

TArray<UFlightPointData*> AFlightVisualizer::GetFlightPointsForListView()
{
    TArray<UFlightPointData*> DataList;

    for (int32 i = 0; i < ParsedGPSPoints.Num(); i++)
    {
        UFlightPointData* DataObj = NewObject<UFlightPointData>(this);
        DataObj->PointData = ParsedGPSPoints[i];
        DataObj->Index = i + 1;


        //Tinh toan vi tri the gioi 3d
        if (CoordinateSystem)
        {
            FVector LocalPos = CoordinateSystem->ConvertSingle(ParsedGPSPoints[i]);
			DataObj->WorldLocation = LocalPos * WorldScale;
        }

        // [THÊM MỚI] Tính toán thông tin tới điểm tiếp theo
        if (i < ParsedGPSPoints.Num() - 1)
        {
            const FFlightPoint& CurrentP = ParsedGPSPoints[i];
            const FFlightPoint& NextP = ParsedGPSPoints[i + 1];

            // Tính khoảng cách (Haversine hoặc Euclidean tùy bạn chọn, ở đây dùng Haversine cho chuẩn GPS)
            DataObj->DistanceToNext = (float)UFlightMathLibrary::HaversineDistance(CurrentP, NextP);

            // Tính vận tốc
            DataObj->VelocityToNext = (float)UFlightMathLibrary::ComputeInstanVeclocity(CurrentP, NextP);

            // Tính góc phương vị
            DataObj->BearingToNext = (float)UFlightMathLibrary::ComputeBearing(CurrentP, NextP);
        }
        else
        {
            // Điểm cuối cùng không có điểm tiếp theo
            DataObj->DistanceToNext = 0.0f;
            DataObj->VelocityToNext = 0.0f;
            DataObj->BearingToNext = 0.0f;
        }

        DataList.Add(DataObj);
    }

    return DataList;
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

