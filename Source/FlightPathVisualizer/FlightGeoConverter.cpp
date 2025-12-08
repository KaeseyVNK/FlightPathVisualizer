// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightGeoConverter.h"
#include "GeoReferencingSystem.h"
#include "DrawDebugHelpers.h"


// Sets default values
AFlightGeoConverter::AFlightGeoConverter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AFlightGeoConverter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlightGeoConverter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

//===================================================================
// Set Origin tu GPS point dau tien
//===================================================================
void AFlightGeoConverter::SetOriginFromFirstPoint(const FFlightPoint& FirstPoint)
{
	if (!GeoSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("FlightGeoConverter: GeoSystem is not assigned!"));
		return;
	}

	// Longitude, Latitude, Altitude
	OriginGeographic = FGeographicCoordinates(FirstPoint.Longitude, FirstPoint.Latitude, FirstPoint.Altitude);

	UE_LOG(LogTemp, Log, TEXT("FlightGeoConverter: Origin set to Lat: %f, Lon: %f, Alt: %f"),
		OriginGeographic.Latitude, OriginGeographic.Longitude, OriginGeographic.Altitude);
}

//===================================================================
// chuyen doi mang GPS sang toa do UE
//===================================================================
bool AFlightGeoConverter::ConvertGPSArrayToUE(const TArray<FFlightPoint>& GPSPoints, TArray<FVector>& OutLocalPoints)
{
	if (!GeoSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("FlightGeoConverter: GeoSystem is not assigned!"));
		return false;
	}

	if (GPSPoints.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("FlightGeoConverter: Input GPSPoints array is empty!"));
		return false;
	}

	OutLocalPoints.Empty();
	
	UWorld* World = GetWorld();
	
	for (int32 i = 0; i < GPSPoints.Num(); i++)
	{
		const FFlightPoint& Point = GPSPoints[i];
		FVector LocalPos = ConvertSingleGPS(Point);
		OutLocalPoints.Add(LocalPos);
		
		// Ve hinh tron debug tai vi tri da chuyen doi
		if (World)
		{
			DrawDebugSphere(
				World,
				LocalPos,
				50.0f,                              // Ban kinh hinh tron (cm)
				12,                                 // So doan tron
				FColor::Green,                      // Mau xanh la
				false,                              // Khong persistent
				36700.0f,                              // Thoi gian hien thi (giay)
				0,                                  // Depth priority
				2.0f                                // Thickness
			);
			
			UE_LOG(LogTemp, Log, TEXT("[ConvertGPS] Point %d: GPS(%.6f, %.6f, %.2f) -> Local(%.2f, %.2f, %.2f)"),
				i + 1, Point.Latitude, Point.Longitude, Point.Altitude,
				LocalPos.X, LocalPos.Y, LocalPos.Z);
		}
	}

	return true;
}

//===================================================================
// chuyen doi 1 diem GPS sang toa do UE
//===================================================================
FVector AFlightGeoConverter::ConvertSingleGPS(const FFlightPoint& GPSPoint)
{
	//if(!GeoSystem)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("FlightGeoConverter: GeoSystem is not assigned!"));
	//	return FVector();
	//}

	////Tao toa do dia ly 
	//FGeographicCoordinates TargetGeo(GPSPoint.Longitude, GPSPoint.Latitude, GPSPoint.Altitude);


	////Chuyen sang ECEF
	//FVector TargetECEF, OriginECEF;
	//GeoSystem->GeographicToECEF(TargetGeo, TargetECEF);
	//GeoSystem->GeographicToECEF(OriginGeographic, OriginECEF);

	//FVector LocalECEF = TargetECEF - OriginECEF;

	//FVector LocalUE;
	//GeoSystem->ECEFToEngine(LocalECEF, LocalUE);
	//	

	//return LocalUE;

	if (!GeoSystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("FlightGeoConverter: GeoSystem is not assigned!"));
		return FVector::ZeroVector;
	}

	FGeographicCoordinates TargetGeo(GPSPoint.Longitude, GPSPoint.Latitude, GPSPoint.Altitude);

	FVector TargetECEF, OriginECEF;
	GeoSystem->GeographicToECEF(TargetGeo, TargetECEF);
	GeoSystem->GeographicToECEF(OriginGeographic, OriginECEF);

	FVector LocalECEF = TargetECEF - OriginECEF;

	FVector LocalUE;
	GeoSystem->ECEFToEngine(LocalECEF, LocalUE);

	return LocalUE;
}

