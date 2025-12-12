// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "FlightPoint.h"
#include "FlightCoordinateActor.h"
#include "FlightPathSplineActor.h"
#include "FlightPointData.h"

#include "FlightVisualizer.generated.h"

// [THÊM MỚI] Struct chứa thông tin thống kê để gửi cho UI
USTRUCT(BlueprintType)
struct FFlightStats
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    float TotalDistanceKm;

    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    float TotalFlightTimeSec;

    UPROPERTY(BlueprintReadOnly, Category = "Stats")
    float AvgAltitudeMeters;
};

UCLASS()
class FLIGHTPATHVISUALIZER_API AFlightVisualizer : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFlightVisualizer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AFlightGeoConverter* GeoConverter;

	UFUNCTION(BlueprintCallable )
	void LoadAndVisualizeFlightPath(const FString& CSVPath);*/

	// ============================================
	// Convert  visualize
	// ============================================
	UFUNCTION(BlueprintCallable, Category = "Flight")
	void LoadAndVisualizeFlightPath(const FString& CSVPath);

	// Actor chuyen doi GPS ENU
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	AFlightCoordinateActor* CoordinateSystem;


	//============================
	// Cai dat cua hien thi duong bay
	//============================
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float WorldScale = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float Thickness = 20.0f;

	// Mesh dùng để hiển thị đường nối (chọn Cylinder/Pipe)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	UStaticMesh* SplineMesh;

	// Mesh dùng để hiển thị các điểm Waypoint (chọn Sphere)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	UStaticMesh* WaypointMesh;

	// Kích thước điểm Waypoint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	FVector WaypointScale = FVector(1.0f);

	// Material dùng để hiển thị đường nối
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	UMaterialInterface* SplineMaterial;

    // [THÊM MỚI] Biến này dùng để lưu giữ Actor đường bay hiện tại
    UPROPERTY()
    AFlightPathSplineActor* CurrentSplineActor;

    //===========================
	//Stats cua chuyen bay
	//===========================
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flight Stats")
	float TotalDistanceKm;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flight Stats")
	float TotalFlightTimeSec;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flight Stats")
	float AvgAltitudeMeters;

	void CaculateFlightStats(const TArray<FFlightPoint>& GPSPoints);


	//chuyen doi du lieu cho ui listview
	UFUNCTION(BlueprintCallable, Category = "Flight UI")
	TArray<UFlightPointData*> GetFlightPointsForListView();



	UPROPERTY(BlueprintReadOnly, Category = "Flight Data")
	TArray<FFlightPoint> ParsedGPSPoints;

	UFUNCTION(BlueprintCallable, Category = "Flight Data")
	TArray<FFlightPoint> GetFlightPoints() const { return ParsedGPSPoints; }

public:
    // Hàm trả về struct thống kê cho Widget
    UFUNCTION(BlueprintPure, Category = "Flight Stats")
    FFlightStats GetFlightStatistics() const;

private:
	void DrawDebugPoints(const TArray<FVector>& Points);

};
