#include "Misc/AutomationTest.h"
#include "GeoReferencingSystem.h"
#include "FlightGeoConverter.h"
#include "FlightPoint.h"
#include "FlightMathLibrary.h"


// ============================================================================
// ROUND-TRIP TEST (GPS → Local → GPS)
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFlightGeo_RoundTripTest,
    "Flight.Geo.RoundTrip",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

    bool FFlightGeo_RoundTripTest::RunTest(const FString& Parameters)
{
    // Create standalone world (NO EDITOR API)
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull(TEXT("World must not be null"), World);

    // Spawn actors
    AGeoReferencingSystem* GeoSys = World->SpawnActor<AGeoReferencingSystem>();
    AFlightGeoConverter* Converter = World->SpawnActor<AFlightGeoConverter>();

    TestNotNull(TEXT("GeoSystem must not be null"), GeoSys);
    TestNotNull(TEXT("Converter must not be null"), Converter);

    Converter->GeoSystem = GeoSys;

    // Test GPS input
    FFlightPoint P0("T", 10.780550, 106.687877, 2.0);
    Converter->SetOriginFromFirstPoint(P0);

    // Convert forward
    FVector Local = Converter->ConvertSingleGPS(P0);

    // Convert backward
    FVector BackECEF;
    GeoSys->EngineToECEF(Local, BackECEF);

    FGeographicCoordinates BackGeo;
    GeoSys->ECEFToGeographic(BackECEF, BackGeo);

    // Validate tiny drift
    TestTrue(TEXT("Latitude roundtrip correct"),
        FMath::Abs(BackGeo.Latitude - P0.Latitude) < 0.000001);

    TestTrue(TEXT("Longitude roundtrip correct"),
        FMath::Abs(BackGeo.Longitude - P0.Longitude) < 0.000001);

    return true;
}



// ============================================================================
// DISTANCE TEST (Haversine ≈ UE Local Distance)
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFlightGeo_DistanceTest,
    "Flight.Geo.Distance",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

    bool FFlightGeo_DistanceTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    TestNotNull(TEXT("World must not be null"), World);

    AGeoReferencingSystem* GeoSys = World->SpawnActor<AGeoReferencingSystem>();
    AFlightGeoConverter* Converter = World->SpawnActor<AFlightGeoConverter>();

    TestNotNull(TEXT("GeoSystem must not be null"), GeoSys);
    TestNotNull(TEXT("Converter must not be null"), Converter);

    Converter->GeoSystem = GeoSys;

    // Two nearby GPS points
    FFlightPoint A("T1", 10.780550, 106.687877, 1.0);
    FFlightPoint B("T2", 10.780551, 106.687878, 1.0);

    Converter->SetOriginFromFirstPoint(A);

    FVector LocalA = Converter->ConvertSingleGPS(A);
    FVector LocalB = Converter->ConvertSingleGPS(B);

    double D_local = FVector::Distance(LocalA, LocalB);
    double D_gps = UFlightMathLibrary::HaversineDistance(A, B);

    // Acceptable tolerance ~1 meter
    TestTrue(TEXT("Local distance ≈ GPS distance"),
        FMath::Abs(D_local - D_gps) < 1.0);

    return true;
}



// ============================================================================
// BEARING TEST
// ============================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FFlightGeo_BearingTest,
    "Flight.Geo.Bearing",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

    bool FFlightGeo_BearingTest::RunTest(const FString& Parameters)
{
    FFlightPoint A("T1", 10.780000, 106.687000, 0);
    FFlightPoint B("T2", 10.781000, 106.687000, 0); // Due North

    double Bearing = UFlightMathLibrary::ComputeBearing(A, B);

    TestTrue(TEXT("Bearing ≈ 0° for north movement"),
        FMath::IsNearlyEqual(Bearing, 0.0, 5.0));

    return true;
}
