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
    //MetersPerLat = 111320.0;

    //// lon thay đổi theo cos(latitude)
    //MetersPerLon = 111320.0 * FMath::Cos(Lat0Rad);

    //bInitialized = true;

    //UE_LOG(LogTemp, Log, TEXT("[CoordActor] NED-Projection Origin Set Lat %.6f Lon %.6f Alt %.2f"),
    //    Origin_Lat, Origin_Lon, Origin_Alt);


     // ============================================================
    // Local Tangent Plane — WGS84 ellipsoid projection
    // ============================================================

    // WGS84 constants
    const double a = 6378137.0;              // Semi-major axis (m)
    const double e2 = 0.00669437999014;      // Eccentricity squared

    // Calculate radius of curvature for latitude (Meridian)
    double sinLat = FMath::Sin(Lat0Rad);
    double denominator = 1.0 - e2 * sinLat * sinLat;
    double sqrtDenom = FMath::Sqrt(denominator);

    // Meridian radius of curvature (North-South direction)
    MetersPerLat = (a * (1.0 - e2)) / (denominator * sqrtDenom);
    MetersPerLat = MetersPerLat * (PI / 180.0); // Convert to meters per degree

    // Prime vertical radius of curvature (East-West direction)
    double N = a / sqrtDenom;
    MetersPerLon = N * FMath::Cos(Lat0Rad) * (PI / 180.0);

    bInitialized = true;

    UE_LOG(LogTemp, Log, TEXT("[CoordActor] NED-Projection Origin Set Lat %.6f Lon %.6f Alt %.2f"),
        Origin_Lat, Origin_Lon, Origin_Alt);
    UE_LOG(LogTemp, Log, TEXT("[CoordActor] MetersPerLat: %.2f | MetersPerLon: %.2f"),
        MetersPerLat, MetersPerLon);
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

void AFlightCoordinateActor::TestWGS84Accuracy()
{
    UE_LOG(LogTemp, Warning, TEXT("========== WGS84 FORMULA VALIDATION TEST =========="));
    
    // Known reference values from NIMA (National Imagery and Mapping Agency)
    struct TestCase
    {
        double Lat;
        const TCHAR* Location;
        double ExpectedMetersPerLat;
        double ExpectedMetersPerLon;
    };
    
    TestCase TestCases[] = {
        { 0.0,    TEXT("Equator"),        110574.28,  111319.49 },
        { 10.78,  TEXT("Ho Chi Minh"),    110632.73,  109366.41 },
        { 21.03,  TEXT("Hanoi"),          110915.64,  103951.58 },
        { 45.0,   TEXT("Mid Latitude"),   111131.75,   78846.81 },
        { 90.0,   TEXT("North Pole"),     111693.98,       0.0 }
    };
    
    const double a = 6378137.0;
    const double e2 = 0.00669437999014;
    
    UE_LOG(LogTemp, Warning, TEXT("Location         | Lat      | MetersPerLat (Calc/Exp/Error) | MetersPerLon (Calc/Exp/Error)"));
    UE_LOG(LogTemp, Warning, TEXT("--------------------------------------------------------------------------------------------------------"));
    
    for (const TestCase& Test : TestCases)
    {
        double LatRad = FMath::DegreesToRadians(Test.Lat);
        double sinLat = FMath::Sin(LatRad);
        double denominator = 1.0 - e2 * sinLat * sinLat;
        double sqrtDenom = FMath::Sqrt(denominator);
        
        // Calculate MetersPerLat
        double CalcMetersPerLat = (a * (1.0 - e2)) / (denominator * sqrtDenom);
        CalcMetersPerLat *= (PI / 180.0);
        
        // Calculate MetersPerLon
        double N = a / sqrtDenom;
        double CalcMetersPerLon = N * FMath::Cos(LatRad) * (PI / 180.0);
        
        // Calculate errors
        double LatError = FMath::Abs(CalcMetersPerLat - Test.ExpectedMetersPerLat);
        double LonError = FMath::Abs(CalcMetersPerLon - Test.ExpectedMetersPerLon);
        
        double LatErrorPercent = (LatError / Test.ExpectedMetersPerLat) * 100.0;
        double LonErrorPercent = Test.ExpectedMetersPerLon > 0 ? (LonError / Test.ExpectedMetersPerLon) * 100.0 : 0.0;
        
        UE_LOG(LogTemp, Warning, TEXT("%-16s | %7.2f° | %8.1f / %8.1f / %5.1fm (%.4f%%) | %8.1f / %8.1f / %5.1fm (%.4f%%)"),
            Test.Location,
            Test.Lat,
            CalcMetersPerLat, Test.ExpectedMetersPerLat, LatError, LatErrorPercent,
            CalcMetersPerLon, Test.ExpectedMetersPerLon, LonError, LonErrorPercent);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    
    // Additional test: Check if old fixed method would cause error
    if (bInitialized)
    {
        double OldMetersPerLat = 111320.0;
        double OldMetersPerLon = 111320.0 * FMath::Cos(Lat0Rad);
        
        double LatDiff = FMath::Abs(MetersPerLat - OldMetersPerLat);
        double LonDiff = FMath::Abs(MetersPerLon - OldMetersPerLon);
        
        UE_LOG(LogTemp, Warning, TEXT("Current Origin (Lat %.2f°):"), Origin_Lat);
        UE_LOG(LogTemp, Warning, TEXT("  WGS84 Method: MetersPerLat=%.2f | MetersPerLon=%.2f"), MetersPerLat, MetersPerLon);
        UE_LOG(LogTemp, Warning, TEXT("  Old Fixed:    MetersPerLat=%.2f | MetersPerLon=%.2f"), OldMetersPerLat, OldMetersPerLon);
        UE_LOG(LogTemp, Warning, TEXT("  Difference:   %.2fm (%.3f%%) | %.2fm (%.3f%%)"),
            LatDiff, (LatDiff/OldMetersPerLat)*100.0,
            LonDiff, (LonDiff/OldMetersPerLon)*100.0);
        
        // Estimate error over 100km
        double ErrorAt100km = (LatDiff / 111320.0) * 100000.0;
        UE_LOG(LogTemp, Warning, TEXT("  Estimated error if using old method @ 100km: %.1fm"), ErrorAt100km);
    }
    
    UE_LOG(LogTemp, Warning, TEXT("=================================================="));
}
