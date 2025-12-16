#pragma once

#include "CoreMinimal.h"
#include "FlightPoint.generated.h"

USTRUCT(BlueprintType)
struct FFlightPoint
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Data")
    FString Timestamp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Data")
    double Latitude;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Data")
    double Longitude;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Data")
    float Altitude;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight Data")
    double TimeInSeconds;


    // Default constructor initializes all members
    FFlightPoint()
        : Timestamp(TEXT("00:00:00"))
        , Latitude(0.0)
        , Longitude(0.0)
        , Altitude(0.0f)
		, TimeInSeconds(0.0)
    {
    }

    // Parameterized constructor
    FFlightPoint(const FString& InTimestamp, double InLat, double InLon, float InAlt)
        : Timestamp(InTimestamp)
        , Latitude(InLat)
        , Longitude(InLon)
        , Altitude(InAlt)
        , TimeInSeconds(0.0)
    {
    }
};
