#include "FlightCoordinateActor.h"
#include "FlightMathLibrary.h"


AFlightCoordinateActor::AFlightCoordinateActor()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AFlightCoordinateActor::BeginPlay()
{
    Super::BeginPlay();
}

void AFlightCoordinateActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AFlightCoordinateActor::InitializeFromPoint(const FFlightPoint& OriginPoint)
{
    Origin_Lat = OriginPoint.Latitude;
    Origin_Lon = OriginPoint.Longitude;
    Origin_Alt = OriginPoint.Altitude;

    Lat0Rad = FMath::DegreesToRadians(Origin_Lat);

    // ============================================================
    // Local Tangent Plane — linear projection
    // ============================================================

    // cố định 111320 m mỗi 1 degree lat
    MetersPerLat = 111320.0;

    // lon thay đổi theo cos(latitude)
    MetersPerLon = 111320.0 * FMath::Cos(Lat0Rad);

    bInitialized = true;

    UE_LOG(LogTemp, Log, TEXT("[CoordActor] NED-Projection Origin Set Lat %.6f Lon %.6f Alt %.2f"),
        Origin_Lat, Origin_Lon, Origin_Alt);
}

FVector AFlightCoordinateActor::ConvertSingle(const FFlightPoint& P) const
{
    if (!bInitialized)
    {
        UE_LOG(LogTemp, Error, TEXT("ConvertSingle() called before Initialize!"));
        return FVector::ZeroVector;
    }

    // ============================================================
    // NED (North-East-Down) to Unreal Engine Coordinate System
    // ============================================================

    double dLatDeg = P.Latitude - Origin_Lat;
    double dLonDeg = P.Longitude - Origin_Lon;
    double dAlt = P.Altitude - Origin_Alt;

    // Step 1: Calculate NED coordinates (Aviation standard)
    double N = dLatDeg * MetersPerLat;      // North
    double E = dLonDeg * MetersPerLon;      // East
    double D = -dAlt;                       // Down (negative altitude)

    // Step 2: Map NED to Unreal Engine coordinates
    // Unreal: X=Forward, Y=Right, Z=Up
    // Mapping: X=North, Y=East, Z=-Down (Up)
    return FVector(N, E, -D);
}

void AFlightCoordinateActor::ConvertArray(const TArray<FFlightPoint>& InPoints, TArray<FVector>& OutLocal)
{
    OutLocal.Empty();

    if (!bInitialized)
    {
        UE_LOG(LogTemp, Error, TEXT("ConvertArray() called before Initialize!"));
        return;
    }

    for (const FFlightPoint& P : InPoints)
    {
        OutLocal.Add(ConvertSingle(P));
    }
}

FVector AFlightCoordinateActor::GeneratePositionFromBearing(const FFlightPoint& StartPoint, double BearingDeg, double DistanceMeters, double AltitudeDelta) const
{
    if (!bInitialized)
    {
        UE_LOG(LogTemp, Error, TEXT("GeneratePositionFromBearing() called before Initialize!"));
        return FVector::ZeroVector;
    }

    // Step 1: Convert StartPoint to Unreal coordinates
    FVector StartPos = ConvertSingle(StartPoint);

    // Step 2: Calculate displacement in NED from Bearing and Distance
    double BearingRad = FMath::DegreesToRadians(BearingDeg);
    
    // NED: Bearing 0° = North (+X), 90° = East (+Y)
    double DeltaN = DistanceMeters * FMath::Cos(BearingRad);  // North component
    double DeltaE = DistanceMeters * FMath::Sin(BearingRad);  // East component
    double DeltaZ = AltitudeDelta;                             // Altitude change (already in Up direction)

    // Step 3: Add displacement to start position
    // QUAN TRỌNG: Vì StartPos đã ở trong Unreal coordinate (X=N, Y=E, Z=Up),
    // ta chỉ cần cộng trực tiếp
    return FVector(StartPos.X + DeltaN, StartPos.Y + DeltaE, StartPos.Z + DeltaZ);
}

void AFlightCoordinateActor::TestDeadReckoningAccuracy(const TArray<FFlightPoint>& GPSPoints)
{
    if (!bInitialized || GPSPoints.Num() < 2)
    {
        UE_LOG(LogTemp, Error, TEXT("TestDeadReckoningAccuracy() requires initialization and at least 2 points!"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("========== DEAD RECKONING ACCURACY TEST =========="));
    UE_LOG(LogTemp, Warning, TEXT("Comparing GPS-based vs Bearing-based positioning"));
    
    double TotalError = 0.0;
    double MaxError = 0.0;
    int32 TestedPoints = 0;

    for (int32 i = 1; i < GPSPoints.Num(); i++)
    {
        const FFlightPoint& PrevPoint = GPSPoints[i - 1];
        const FFlightPoint& CurrentPoint = GPSPoints[i];

        // Method 1: Direct GPS conversion (Ground Truth)
        FVector GPSBasedPos = ConvertSingle(CurrentPoint);

        // Method 2: Dead Reckoning from previous point
        double Distance = UFlightMathLibrary::HaversineDistance(PrevPoint, CurrentPoint);
        double Bearing = UFlightMathLibrary::ComputeBearing(PrevPoint, CurrentPoint);
        double AltDelta = CurrentPoint.Altitude - PrevPoint.Altitude;

        FVector BearingBasedPos = GeneratePositionFromBearing(PrevPoint, Bearing, Distance, AltDelta);

        // Calculate error
        double Error = FVector::Dist(GPSBasedPos, BearingBasedPos);
        TotalError += Error;
        MaxError = FMath::Max(MaxError, Error);
        TestedPoints++;

        // Log detailed comparison
        UE_LOG(LogTemp, Log, TEXT("Point %d:"), i);
        UE_LOG(LogTemp, Log, TEXT("  GPS: Lat=%.6f Lon=%.6f Alt=%.2f"), 
            CurrentPoint.Latitude, CurrentPoint.Longitude, CurrentPoint.Altitude);
        UE_LOG(LogTemp, Log, TEXT("  GPS-based Pos:     %s"), *GPSBasedPos.ToString());
        UE_LOG(LogTemp, Log, TEXT("  Bearing-based Pos: %s"), *BearingBasedPos.ToString());
        UE_LOG(LogTemp, Log, TEXT("  Bearing: %.2f° | Distance: %.2fm | Error: %.4fm"), 
            Bearing, Distance, Error);

        // Warning if error is significant
        if (Error > 1.0) // 1 meter threshold
        {
            UE_LOG(LogTemp, Warning, TEXT("  ⚠️ Large error detected: %.4fm"), Error);
        }
    }

    double AvgError = (TestedPoints > 0) ? (TotalError / TestedPoints) : 0.0;

    UE_LOG(LogTemp, Warning, TEXT("========== TEST RESULTS =========="));
    UE_LOG(LogTemp, Warning, TEXT("Points Tested: %d"), TestedPoints);
    UE_LOG(LogTemp, Warning, TEXT("Average Error: %.4f meters"), AvgError);
    UE_LOG(LogTemp, Warning, TEXT("Max Error: %.4f meters"), MaxError);
    UE_LOG(LogTemp, Warning, TEXT("=================================="));
}
