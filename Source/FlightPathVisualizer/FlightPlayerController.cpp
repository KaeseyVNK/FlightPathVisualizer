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
