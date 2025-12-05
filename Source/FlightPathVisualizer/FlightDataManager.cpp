// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightDataManager.h"

//-> Cung cap API mo rong hop thoai windows explorer
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"


//-> Cung cap API doc file tu o cung
#include "Misc/FileHelper.h"

//-> Cung cap API xu ly duong dan file
#include "Misc/Paths.h"

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
