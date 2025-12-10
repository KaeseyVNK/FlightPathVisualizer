// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlightPoint.h"
#include "FlightCoordinateActor.h"
#include "FlightPathSplineActor.h"
#include "FlightVisualizer.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float WorldScale = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	float Thickness = 20.0f;

	// [SỬA] Mesh dùng để hiển thị đường nối (chọn Cylinder/Pipe)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	UStaticMesh* SplineMesh;

	// [THÊM MỚI] Mesh dùng để hiển thị các điểm Waypoint (chọn Sphere)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	UStaticMesh* WaypointMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	FVector WaypointScale = FVector(1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
	UMaterialInterface* SplineMaterial;

private:
	void DrawDebugPoints(const TArray<FVector>& Points);

};
