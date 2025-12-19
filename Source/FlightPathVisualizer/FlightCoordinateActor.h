#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlightPoint.h"
#include "FlightCoordinateActor.generated.h"

UCLASS()
class FLIGHTPATHVISUALIZER_API AFlightCoordinateActor : public AActor
{
    GENERATED_BODY()

public:
    AFlightCoordinateActor();

    // Origin GPS
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GPS")
    double Origin_Lat;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GPS")
    double Origin_Lon;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GPS")
    double Origin_Alt;

    bool bInitialized = false;

private:

    double Lat0Rad;
    double MetersPerLat;
    double MetersPerLon;

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // Initialize from first GPS
    UFUNCTION(BlueprintCallable, Category = "GPS")
    void InitializeFromPoint(const FFlightPoint& OriginPoint);

    // Convert one GPS → Local flat coordinates
    UFUNCTION(BlueprintCallable, Category = "GPS")
    FVector ConvertSingle(const FFlightPoint& P) const;

    // Convert array
    UFUNCTION(BlueprintCallable, Category = "GPS")
    void ConvertArray(const TArray<FFlightPoint>& InPoints, TArray<FVector>& OutLocal);

    // Test: Generate position from bearing, distance, altitude
    UFUNCTION(BlueprintCallable, Category = "GPS")
    FVector GeneratePositionFromBearing(const FFlightPoint& StartPoint, double BearingDeg, double DistanceMeters, double AltitudeDelta) const;

    // Test: Compare GPS-based vs Bearing-based coordinates
    UFUNCTION(BlueprintCallable, Category = "GPS")
    void TestDeadReckoningAccuracy(const TArray<FFlightPoint>& GPSPoints);
};
