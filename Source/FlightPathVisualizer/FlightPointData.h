// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "FlightPoint.h"

#include "FlightPointData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class FLIGHTPATHVISUALIZER_API UFlightPointData : public UObject
{
	GENERATED_BODY()

public:

	//Du lieu diem bay
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data", Meta = (MakeEditWidget = true))
	FFlightPoint PointData;

	//so thu tu indexs
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data", Meta = (MakeEditWidget = true))
	int32 Index;

	//Them vi tri thuc te trong the gioi 3d
	UPROPERTY(BlueprintReadOnly, Category = "Flight Data")
	FVector WorldLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	//m
	float DistanceToNext; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	//m/s
	float VelocityToNext;

	//Do 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	float BearingToNext;

};

