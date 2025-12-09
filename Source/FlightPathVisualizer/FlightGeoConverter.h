// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "GeoReferencingSystem.h"
#include "GeographicCoordinates.h"

#include "FlightPoint.h"

#include "FlightGeoConverter.generated.h"

UCLASS()
class FLIGHTPATHVISUALIZER_API AFlightGeoConverter : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlightGeoConverter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Geo Referencing System tu level
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeoReferencing")
	AGeoReferencingSystem* GeoSystem;

	//Origin Geographic Coordinates
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeoReferencing")
	FGeographicCoordinates OriginGeographic;

	// Dat Origin lam diem dau tien 
	UFUNCTION(BlueprintCallable, Category = "GeoReferencing")
	void SetOriginFromFirstPoint(const FFlightPoint& FirstPoint);

	UFUNCTION(BlueprintCallable, Category = "GeoReferencing")
	bool ConvertGPSArrayToUE(const TArray<FFlightPoint>& GPSPoints, TArray<FVector>& OutLocalPoints);

	FVector ConvertSingleGPS(const FFlightPoint& GPSPoint);

};
