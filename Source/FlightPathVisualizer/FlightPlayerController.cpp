// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"

AFlightPlayerController::AFlightPlayerController()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;

}



void AFlightPlayerController::TeleportCameraToLocation(FVector TargetLocation, float Bearing)
{
	APawn* ControlledPawn = GetPawn();
	if (ControlledPawn)
	{
		//FVector CameraOffset = FVector(0, 0, 500.0f); // Đặt camera cao hơn 500 đơn vị so với vị trí mục tiêu
		//ControlledPawn->SetActorLocation(TargetLocation);

		////Neu muon camera nhin xuong diem do, co the set Rotaion 
		////ControlledPawn->SetActorRotation(FRotator(-90.0f, 0.0f, 0.0f));

		//--------------------------------------------------------------------------------------------
		// 1. Tính toán hướng bay (Flight Direction) từ Bearing
		// Lưu ý: Trong hệ tọa độ của bạn (dựa trên FlightCoordinateActor):
		// Bearing 0 (Bắc) -> Y+ 
		// Bearing 90 (Đông) -> X+

		float BearingRad = FMath::DegreesToRadians(Bearing);
		FVector FlightDir(FMath::Sin(BearingRad), FMath::Cos(BearingRad), 0.0f);

		// 2. Tính toán vector bên phải (Right Vector) vuông góc với hướng bay
		// Nếu FlightDir là (x, y), thì RightVector là (y, -x)
		FVector RightDir(FlightDir.Y, -FlightDir.X, 0.0f);

		// 3. Thiết lập khoảng cách Offset
		float SideDistance = 200.0f; // Cách sang bên 30 mét (3000 units)
		float HeightOffset = 100.0f; // Cao hơn 20 mét (2000 units)

		// Vị trí Camera mới = Mục tiêu + (Sang phải * Khoảng cách) + (Lên cao)
		// Bạn có thể đổi RightDir thành -RightDir nếu muốn camera nằm bên trái
		FVector CameraLocation = TargetLocation + (RightDir * SideDistance) + FVector(0, 0, HeightOffset);

		// 4. Di chuyển Pawn tới vị trí mới
		ControlledPawn->SetActorLocation(CameraLocation);

		// 5. Xoay Camera để nhìn vào mục tiêu (Look At)
		// Vector hướng từ Camera tới Mục tiêu
		FVector LookAtDir = TargetLocation - CameraLocation;
		FRotator NewRotation = LookAtDir.Rotation();

		// Cập nhật xoay cho Pawn (để mô hình nhân vật quay theo - nếu có)
		ControlledPawn->SetActorRotation(NewRotation);

		// QUAN TRỌNG: Cập nhật xoay cho Controller (để Camera quay theo)
		SetControlRotation(NewRotation);
	}
}

void AFlightPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Add the default mapping context
	if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void AFlightPlayerController::SetupInputComponent()
{
	// Call the correct parent implementation for PlayerControllers
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(ShowMouseAction, ETriggerEvent::Started, this, &AFlightPlayerController::OnShowMouseTrigged);
		EnhancedInputComponent->BindAction(ShowMouseAction, ETriggerEvent::Completed, this, &AFlightPlayerController::OnShowMouseCompeted);
		
		if (ToggleListAction)
		{
			EnhancedInputComponent->BindAction(ToggleListAction, ETriggerEvent::Started, this, &AFlightPlayerController::OnToggleListTrigged);
		}
	
	}
}

void AFlightPlayerController::OnShowMouseTrigged(const FInputActionValue& Value)
{
	// Hien chuot
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
}

void AFlightPlayerController::OnShowMouseCompeted(const FInputActionValue& Value)
{   // An chuot
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void AFlightPlayerController::OnToggleListTrigged(const FInputActionValue& Value)
{
	// Logic Toggle UI
	// Cách 1: Nếu bạn lưu biến Widget trong Controller (cần khai báo biến MainWidget trước)
	/*
	if (MainWidget)
	{
		if (MainWidget->IsInViewport())
		{
			MainWidget->RemoveFromParent(); // Hoặc SetVisibility(Hidden)
		}
		else
		{
			MainWidget->AddToViewport(); // Hoặc SetVisibility(Visible)
		}
	}
	*/

	// Cách 2 (Dễ nhất cho Blueprint): Gọi một Blueprint Implementable Event
	// Để bạn tự xử lý logic UI bên Blueprint của Controller
	ToggleFlightListUI();
}
