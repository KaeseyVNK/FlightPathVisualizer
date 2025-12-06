// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FlightPoint.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlightMathLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTPATHVISUALIZER_API UFlightMathLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public: 

	//tinh khoang cach giua 2 diem tren mat cau 
	UFUNCTION(BlueprintCallable, Category ="FlightMath")
	static double HaversineDistance(const FFlightPoint& A, const FFlightPoint& B);

	//tinh tong quan duong (m)
	UFUNCTION(BlueprintCallable, Category = "FlightMath")
	static double ComputeCumulativeDistance(const TArray<FFlightPoint>& Points);

	//tinh van toc giua 2 diem (m/s)
	UFUNCTION(BlueprintCallable, Category = "FlightMath")
	static double ComputeInstanVeclocity(const FFlightPoint& A, const FFlightPoint& B);

	//tinh goc phuong vi
	UFUNCTION(BlueprintCallable, Category = "FlightMath")
	static double ComputeBearing(const FFlightPoint& A, const FFlightPoint& B);


	//Helper: convert HH:MM:SS to seconds
	static int32 TimeStringToSeconds(const FString& Timestamp);
	
};
