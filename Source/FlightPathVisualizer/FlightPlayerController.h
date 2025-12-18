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

UENUM(BlueprintType)
enum class EFlightCameraMode : uint8 
{
	FreeFly		UMETA(DisplayName = "Free Fly"),
	RTS 		UMETA(DisplayName = "RTS Mode"),
};


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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* CameraZoomAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ToggleCameraAction;

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

public:

	//Ben luu trang thai camera hien tai
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera Mode")
	EFlightCameraMode CurrentCameraMode = EFlightCameraMode::FreeFly;

	//Do cao mong muon khi o che do RTS 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTS Camera")
	float RTSHeight = 5000.0f;

	//Goc nhin xuong khi o che do RTS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTS Camera")
	float RTSPitchAngle = -60.0f;

	//Toc do zoom khi o che do RTS
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTS Camera")
	float ZoomSpeed = 500.0f;

	// Hàm chuyển đổi chế độ (Gọi từ Input Action)
	UFUNCTION(BlueprintCallable, Category = "Camera Mode")
	void ToggleCameraMode();

	// Hàm xử lý Zoom (Gọi từ Input Action: Mouse Wheel)
	UFUNCTION(BlueprintCallable, Category = "Camera Mode")
	void OnZoomCamera(const FInputActionValue& Value);

private:
    void PerformInteractionTrace();

	// Hàm cập nhật vị trí RTS trong Tick
	void UpdateRTSCamera(float DeltaTime);

    UPROPERTY()
    AFlightVisualizer* CachedVisualizer;
};
