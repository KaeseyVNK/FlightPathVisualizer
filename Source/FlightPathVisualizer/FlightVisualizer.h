// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FlightDataManager.h"
#include "FlightGeoConverter.h"
#include "FlightVisualizer.generated.h"

UCLASS()
class FLIGHTPATHVISUALIZER_API AFlightVisualizer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlightVisualizer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AFlightGeoConverter* GeoConverter;

	UFUNCTION(BlueprintCallable )
	void LoadAndVisualizeFlightPath(const FString& CSVPath);

};
