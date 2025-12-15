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
class AFlightVisualizer;


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

	UFUNCTION(BlueprintCallable, Category = "Flight Control")
	void TeleportCameraToLocation(FVector TargetLocation, float Bearing = 0.0f);

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	//Widget class de hien thi thong tin
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> WaypointInfoWidgetClass;

	//bien luu instance cua widget
	UPROPERTY()
	UUserWidget* WaypointInfoWidget;

	void OnShowMouseTrigged(const FInputActionValue& Value);
	void OnShowMouseCompeted(const FInputActionValue& Value);
	void OnToggleListTrigged(const FInputActionValue& Value);
	
private:
    void PerformInteractionTrace();

    UPROPERTY()
    AFlightVisualizer* CachedVisualizer;
};
