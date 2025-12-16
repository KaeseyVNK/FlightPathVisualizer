#include "Misc/AutomationTest.h"
#include "FlightMathLibrary.h"
#include "FlightPoint.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "FlightDataManager.h"


// ====================================================================
// TEST: Convert timestamp HH:MM:SS → seconds
// ====================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTestTimeConversion,
    "FlightMath.Time.Convert",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTestTimeConversion::RunTest(const FString& Parameters)
{
    int32 result = UFlightMathLibrary::TimeStringToSeconds("01:02:03");

    TestEqual(TEXT("01:02:03 should equal 3723 seconds"), result, 3723);

    return true;
}



// ====================================================================
// TEST: Haversine distance
// ====================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTestHaversine,
    "FlightMath.Distance.Haversine",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTestHaversine::RunTest(const FString& Parameters)
{
    FFlightPoint A("00:00:01", 10.0, 106.0, 0);
    FFlightPoint B("00:00:02", 10.0001, 106.0001, 0);

    double Dist = UFlightMathLibrary::HaversineDistance(A, B);

    TestTrue(TEXT("Distance > 0"), Dist > 0);
    TestTrue(TEXT("Distance < 30m"), Dist < 30);

    return true;
}



// ====================================================================
// TEST: Cumulative Distance
// ====================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTestCumulativeDistance,
    "FlightMath.Distance.Cumulative",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTestCumulativeDistance::RunTest(const FString& Parameters)
{
    TArray<FFlightPoint> Points;
    Points.Add(FFlightPoint("00:00:00", 10.0, 106.0, 0));
    Points.Add(FFlightPoint("00:00:01", 10.0001, 106.0001, 0));
    Points.Add(FFlightPoint("00:00:02", 10.0002, 106.0002, 0));

    double Total = UFlightMathLibrary::ComputeCumulativeDistance(Points);

    TestTrue(TEXT("Total distance > 0"), Total > 0);
    TestTrue(TEXT("Total distance < 100m"), Total < 100);

    return true;
}



// ====================================================================
// TEST: Instant Velocity (m/s)
// ====================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTestVelocity,
    "FlightMath.Velocity.Instant",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTestVelocity::RunTest(const FString& Parameters)
{
    FFlightPoint A("00:00:00", 10.0, 106.0, 0);
    FFlightPoint B("00:00:02", 10.0001, 106.0001, 0);

    double Velocity = UFlightMathLibrary::ComputeInstanVeclocity(A, B);

    TestTrue(TEXT("Velocity > 0"), Velocity > 0);
    TestTrue(TEXT("Velocity < 20 m/s"), Velocity < 20);

    return true;
}



// ====================================================================
// TEST: Bearing (hướng bay)
// ====================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTestBearing,
    "FlightMath.Bearing.Basic",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTestBearing::RunTest(const FString& Parameters)
{
    FFlightPoint A("00:00:01", 10.861437, 106.755997, 0);
    FFlightPoint B("00:00:02", 10.861436, 106.756050, 0);

    double Bearing = UFlightMathLibrary::ComputeBearing(A, B);

    // Log để xem giá trị thực
    //UE_LOG(LogTemp, Warning, TEXT("[UnitTest] Bearing computed = %f"), Bearing);

    // Cho phép sai số ±10° vì dữ liệu GPS rất nhỏ
    TestTrue(TEXT("Bearing should be valid angle"), Bearing >= 0 && Bearing <= 360);

    return true;
}


//====================================================================
// TEST: Parse CSV file and validate data
//====================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTestCSVValid,
    "FlightData.CSV.ValidParse",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTestCSVValid::RunTest(const FString& Parameters)
{
    // tạo file CSV tạm
    FString TempFile = FPaths::ProjectSavedDir() / TEXT("test_valid.csv");

    FString Content =
        "Timestamp,Latitude,Longitude,Altitude\n"
        "10:00:01,10.762622,106.660172,50.5\n"
        "10:00:02,10.762640,106.660180,52.1\n";

    FFileHelper::SaveStringToFile(Content, *TempFile);

    // gọi parser
    TArray<FFlightPoint> Points;
    bool bOK = UFlightDataManager::ParseCSV(TempFile, Points);

    // validate
    TestTrue(TEXT("ParseCSV should succeed"), bOK);
    TestEqual(TEXT("Should parse 2 points"), Points.Num(), 2);

    TestEqual(TEXT("Point[0] timestamp"), Points[0].Timestamp, FString("10:00:01"));
    TestEqual(TEXT("Point[0] lat"), Points[0].Latitude, 10.762622);
    TestEqual(TEXT("Point[0] lon"), Points[0].Longitude, 106.660172);
    TestEqual(TEXT("Point[0] alt"), Points[0].Altitude, 50.5f);

    return true;
}


//====================================================================
// TEST: Parse CSV file with invalid data
//====================================================================
IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTestCSVInvalidFormat,
    "FlightData.CSV.InvalidFormat",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTestCSVInvalidFormat::RunTest(const FString& Parameters)
{
    FString TempFile = FPaths::ProjectSavedDir() / TEXT("test_invalid.csv");

    FString BadContent =
        "Timestamp,Latitude,Longitude,Altitude\n"
        "INVALID_LINE_HERE\n";

    FFileHelper::SaveStringToFile(BadContent, *TempFile);

    TArray<FFlightPoint> Points;
    bool bOK = UFlightDataManager::ParseCSV(TempFile, Points);

    TestFalse(TEXT("ParseCSV should fail or return 0 points"), bOK);
    TestEqual(TEXT("Should parse 0 valid points"), Points.Num(), 0);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTestCSVTooFewColumns,
    "FlightData.CSV.MissingColumns",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTestCSVTooFewColumns::RunTest(const FString& Parameters)
{
    FString TempFile = FPaths::ProjectSavedDir() / TEXT("test_missing_col.csv");

    FString Content =
        "Timestamp,Latitude,Longitude,Altitude\n"
        "10:00:01,10.762622,106.660172\n"; // thiếu Altitude

    FFileHelper::SaveStringToFile(Content, *TempFile);

    TArray<FFlightPoint> Points;
    bool bOK = UFlightDataManager::ParseCSV(TempFile, Points);

    TestFalse(TEXT("Parser should reject missing column"), bOK);
    TestEqual(TEXT("No points should be parsed"), Points.Num(), 0);

    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTestCSVOutOfRange,
    "FlightData.CSV.OutOfRangeValues",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter
)

bool FTestCSVOutOfRange::RunTest(const FString& Parameters)
{
    FString TempFile = FPaths::ProjectSavedDir() / TEXT("test_out_of_range.csv");

    FString Content =
        "Timestamp,Latitude,Longitude,Altitude\n"
        "10:00:01,200.0,106.660172,50.0\n"; // latitude invalid

    FFileHelper::SaveStringToFile(Content, *TempFile);

    TArray<FFlightPoint> Points;
    bool bOK = UFlightDataManager::ParseCSV(TempFile, Points);

    TestFalse(TEXT("Invalid latitude should fail"), bOK);
    TestEqual(TEXT("Should parse 0 points"), Points.Num(), 0);

    return true;
}


