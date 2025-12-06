// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightMathLibrary.h"
#include "Math/UnrealMathUtility.h"
#include <cmath>

static constexpr double EARTH_RADIUS_METERS = 6371e3; // Bán kính trung bình của Trái Đất tính bằng mét


//=============================================
//convert timestamp --> serconds
//=============================================
int32 UFlightMathLibrary::TimeStringToSeconds(const FString& Timestamp)
{
	TArray<FString> Parts; 
	Timestamp.ParseIntoArray(Parts, TEXT(":"), true);
	if (Parts.Num() != 3)
	{
		// Invalid format
		return -1;
	}
	int32 H = FCString::Atoi(*Parts[0]);
	int32 M = FCString::Atoi(*Parts[1]);
	int32 S = FCString::Atoi(*Parts[2]);

	return H * 3600 + M * 60 + S;
}

//=============================================
//khoang cach giua 2 diem tren mat cau Harversine 
//=============================================
double UFlightMathLibrary::HaversineDistance(const FFlightPoint& A, const FFlightPoint& B)
{
	double Lat1 = FMath::DegreesToRadians(A.Latitude);
	double Lat2 = FMath::DegreesToRadians(B.Latitude);

	double dLat = FMath::DegreesToRadians(B.Latitude - A.Latitude);
	double dLon = FMath::DegreesToRadians(B.Longitude - A.Longitude);

	double SinLat = FMath::Sin(dLat / 2);
	double SinLon = FMath::Sin(dLon / 2);

	double a = SinLat * SinLat + FMath::Cos(Lat1) * FMath::Cos(Lat2) * SinLon * SinLon;

	double c = 2 * FMath::Atan2(FMath::Sqrt(a), FMath::Sqrt(1 - a));

	return c * EARTH_RADIUS_METERS;
}

//=============================================
//tong quan duong tich luy
//=============================================
double UFlightMathLibrary::ComputeCumulativeDistance(const TArray<FFlightPoint>& Points)
{
	double Total = 0.0;

	for(int32 i = 1; i < Points.Num(); i++)
	{
		Total += HaversineDistance(Points[i - 1], Points[i]);
	}

	return Total;
}

//=============================================
//van toc giua 2 diem
//=============================================
double UFlightMathLibrary::ComputeInstanVeclocity(const FFlightPoint& A, const FFlightPoint& B)
{

	double Distance = HaversineDistance(A, B);

	int32 TimeA = TimeStringToSeconds(A.Timestamp);
	int32 TimeB = TimeStringToSeconds(B.Timestamp);

	int32 DeltaTime = TimeB - TimeA;

	if (DeltaTime <= 0) {
		return 0.0; // Tránh chia cho 0 hoặc thời gian âm
	}

	return Distance/DeltaTime;
}


//=============================================
//Tinh goc phuong vi (bearing) giua 2 diem
//=============================================

double UFlightMathLibrary::ComputeBearing(const FFlightPoint& A, const FFlightPoint& B)
{
	double Lat1 = FMath::DegreesToRadians(A.Latitude);
	double Lat2 = FMath::DegreesToRadians(B.Latitude);
	double Lon1 = FMath::DegreesToRadians(A.Longitude);
	double Lon2 = FMath::DegreesToRadians(B.Longitude);

	double dLon = Lon2 - Lon1;

	// Dùng std::sin/cos (double precision), KHÔNG dùng FMath (float)
	double y = std::sin(dLon) * std::cos(Lat2);
	double x = std::cos(Lat1) * std::sin(Lat2) -
		std::sin(Lat1) * std::cos(Lat2) * std::cos(dLon);

	double bearingRad = std::atan2(y, x);
	double bearingDeg = FMath::RadiansToDegrees(bearingRad);

	// Chuẩn hóa về 0–360 độ
	bearingDeg = FMath::Fmod(bearingDeg + 360.0, 360.0);

	return bearingDeg;
}

