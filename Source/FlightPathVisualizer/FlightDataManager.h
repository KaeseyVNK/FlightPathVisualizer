// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h" //Cho phép viet ham staic goi tu Blueprint
#include "FlightDataManager.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTPATHVISUALIZER_API UFlightDataManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public: 
	//Hien thi hop thoai chon file cua window --> duong dan file csv duoc chon
	UFUNCTION(BlueprintCallable, Category = "Flight Data")
	static bool OpenCSVFileDialog(FString& OutFilePath);



	//Doc noi dung file csv va in output log ra unreal editor
	UFUNCTION(BlueprintCallable, Category = "Flight Data")
	static bool LoadCSVAndPrint(const FString& FilePath);


	UFUNCTION(BlueprintCallable, Category = "Flight Data")
	static bool ParseCSV(const FString& FilePath, TArray<struct FFlightPoint>& OutPoints);
};
