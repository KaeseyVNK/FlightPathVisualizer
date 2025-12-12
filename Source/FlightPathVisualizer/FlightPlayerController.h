// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "FlightPlayerController.generated.h"

/**
 * 
 */
class UInputMappingContext;
class UInputAction;


UCLASS()
class FLIGHTPATHVISUALIZER_API AFlightPlayerController : public APlayerController
{
	GENERATED_BODY()

public: 
	AFlightPlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ShowMouseAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleListAction;

	UFUNCTION(BlueprintImplementableEvent, Category = "UI")
	void ToggleFlightListUI();

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void OnShowMouseTrigged(const FInputActionValue& Value);
	void OnShowMouseCompeted(const FInputActionValue& Value);
	void OnToggleListTrigged(const FInputActionValue& Value);
	
};
