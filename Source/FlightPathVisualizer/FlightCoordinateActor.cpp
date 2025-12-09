#include "FlightCoordinateActor.h"

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

    UE_LOG(LogTemp, Log, TEXT("[CoordActor] Flat-Projection Origin Set Lat %.6f Lon %.6f Alt %.2f"),
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
    // Calculate flat-projected meters
    // ============================================================

    double dLatDeg = P.Latitude - Origin_Lat;
    double dLonDeg = P.Longitude - Origin_Lon;
    double dAlt = P.Altitude - Origin_Alt;

    double Y = dLatDeg * MetersPerLat;   // North
    double X = dLonDeg * MetersPerLon;   // East
    double Z = dAlt;                     // Altitude preserved exactly

    return FVector(X, Y, Z);
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
