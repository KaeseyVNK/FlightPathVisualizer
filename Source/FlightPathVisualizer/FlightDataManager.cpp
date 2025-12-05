// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightDataManager.h"

//-> Cung cap API mo rong hop thoai windows explorer
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"


//-> Cung cap API doc file tu o cung
#include "Misc/FileHelper.h"

//-> Cung cap API xu ly duong dan file
#include "Misc/Paths.h"

#include "FlightPoint.h"


bool  UFlightDataManager::OpenCSVFileDialog(FString& OutFilePath)
{

	//Kiem tra xem co truy cap duoc module desktop platform khong
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		UE_LOG(LogTemp, Warning, TEXT("Desktop Platform module is not available."));
		return false;
	}

	//Mang chua danh cac file duoc chon
	TArray<FString> SelectedFiles;


	//Hien thi hop thoai chon file
	bool bOpened = DesktopPlatform ->OpenFileDialog(
		nullptr,									//Khong co cua so cha
		TEXT("Chon file CSV"),						//Tieu de hop thoai
		TEXT(""),									//Thu muc mac dinh
		TEXT(""),									//Ten file mac dinh
		TEXT("CSV files (*.csv)|*.csv"),			//Loc file chi hien thi file csv
		EFileDialogFlags::None,						//Khong co co che dac biet
		SelectedFiles								//Mang chua duong dan file duoc chon
	);


	//Kiem tra neu nguoi dung da chon file hay chua
	if(!bOpened || SelectedFiles.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No file was selected."));
		return false;
	}

	//Lay duong dan file dau tien trong mang
	OutFilePath = SelectedFiles[0];

	UE_LOG(LogTemp, Log, TEXT("Selected file: %s"), *OutFilePath);

	return true;
}

bool UFlightDataManager::LoadCSVAndPrint(const FString& FilePath)
{

	UE_LOG(LogTemp, Log, TEXT("Loading file: %s"), *FilePath);


	//Kiem tra xem file co ton tai khong
	if(!FPaths::FileExists(FilePath))
	{
		UE_LOG (LogTemp, Error, TEXT("File does not exist: %s"), *FilePath);
		return false;
	}

	//Mang chua cac dong doc tu file
	TArray<FString> FileLines;


	if(!FFileHelper::LoadFileToStringArray(FileLines, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load file: %s"), *FilePath);
	}


	int countlog = 0;
	//In ra log neu doc file thanh cong
	UE_LOG(LogTemp, Log, TEXT("Successfully loaded file: %s"), *FilePath);
	//In tung dong ra output log
	UE_LOG(LogTemp, Log, TEXT("======= File Content Start ======="));
	for (const FString& Line : FileLines)
	{
		UE_LOG(LogTemp, Log, TEXT("%s"), *Line);
		countlog++;
	}
	UE_LOG(LogTemp, Log, TEXT("======= File Content End ======="));


	UE_LOG(LogTemp, Log, TEXT("======= Total lines read: %d========="), countlog);

	return false;
}

bool UFlightDataManager::ParseCSV(const FString& FilePath, TArray<struct FFlightPoint>& OutPoints)
{
	OutPoints.Empty();

	UE_LOG(LogTemp, Log, TEXT(" [Parsing] CSV file: %s"), *FilePath);

	//Kiem tra xem file co ton tai khong
	if (!FPaths::FileExists(FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("File does not exist: %s"), *FilePath);
		return false;
	}

	//Mang chua cac dong doc tu file
	TArray<FString> Lines;
	if (!FFileHelper::LoadFileToStringArray(Lines, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("[ParseCSV] Failed to read file: %s"), *FilePath);
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("[ParseCSV] %d lines loaded from CSV."), Lines.Num());

	for (int32 i = 1; i < Lines.Num(); i++)
	{
		const FString& Line = Lines[i];

		
		if (Line.TrimStartAndEnd().IsEmpty())
		{
			UE_LOG(LogTemp, Warning, TEXT("[ParseCSV] Skipped empty line %d"), i + 1);
			continue;
		}

		
		TArray<FString> Cells;
		Line.ParseIntoArray(Cells, TEXT(","), true);

		if (Cells.Num() != 4)
		{
			UE_LOG(LogTemp, Error, TEXT("[ParseCSV] Invalid format at line %d: %s"), i + 1, *Line);
			continue;
		}

		FString Timestamp = Cells[0];
		double Lat = FCString::Atod(*Cells[1]);
		double Lon = FCString::Atod(*Cells[2]);
		float Alt = FCString::Atof(*Cells[3]);

		
		if (!Timestamp.Contains(":"))
		{
			UE_LOG(LogTemp, Error, TEXT("[ParseCSV] Invalid timestamp at line %d: %s"), i + 1, *Timestamp);
			continue;
		}

		if (Lat < -90 || Lat > 90)
		{
			UE_LOG(LogTemp, Error, TEXT("[ParseCSV] Invalid latitude at line %d: %f"), i + 1, Lat);
			continue;
		}

		if (Lon < -180 || Lon > 180)
		{
			UE_LOG(LogTemp, Error, TEXT("[ParseCSV] Invalid longitude at line %d: %f"), i + 1, Lon);
			continue;
		}

		
		FFlightPoint Point(Timestamp, Lat, Lon, Alt);
		OutPoints.Add(Point);

		
		UE_LOG(LogTemp, Log, TEXT("[ParseCSV] Line %d    OK ? Timestamp=%s | Lat=%.6f | Lon=%.6f | Alt=%.2f"),
			i + 1, *Timestamp, Lat, Lon, Alt);
	}

	UE_LOG(LogTemp, Log, TEXT("[ParseCSV] Parse complete. Total valid points: %d"), OutPoints.Num());

	return OutPoints.Num() > 0;
}
