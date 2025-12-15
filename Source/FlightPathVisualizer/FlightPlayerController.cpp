// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "DrawDebugHelpers.h"
#include "Components/InstancedStaticMeshComponent.h" 
#include "Kismet/GameplayStatics.h" 
#include "Misc/OutputDeviceNull.h" 

#include "FlightVisualizer.h" 
#include "FlightPoint.h"
#include "FlightMathLibrary.h" // [THÊM MỚI] Include thư viện toán để tính Velocity/Bearing

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
        float BearingRad = FMath::DegreesToRadians(Bearing);
        FVector FlightDir(FMath::Sin(BearingRad), FMath::Cos(BearingRad), 0.0f);

        FVector RightDir(FlightDir.Y, -FlightDir.X, 0.0f);

        float SideDistance = 200.0f; 
        float HeightOffset = 100.0f; 

        FVector CameraLocation = TargetLocation + (RightDir * SideDistance) + FVector(0, 0, HeightOffset);

        ControlledPawn->SetActorLocation(CameraLocation);

        FVector LookAtDir = TargetLocation - CameraLocation;
        FRotator NewRotation = LookAtDir.Rotation();

        ControlledPawn->SetActorRotation(NewRotation);
        SetControlRotation(NewRotation);
    }
}

void AFlightPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    PerformInteractionTrace();
}

void AFlightPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // 1. Tìm FlightVisualizer
    AActor* FoundActor = UGameplayStatics::GetActorOfClass(this, AFlightVisualizer::StaticClass());
    CachedVisualizer = Cast<AFlightVisualizer>(FoundActor);

    // 2. Tạo Widget Instance
    if (WaypointInfoWidgetClass)
    {
        WaypointInfoWidget = CreateWidget<UUserWidget>(this, WaypointInfoWidgetClass);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Chua gan WaypointInfoWidgetClass trong BP_FlightPlayerController!"));
    }

    // 3. Input Mapping
    if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(DefaultMappingContext, 0);
    }
}

void AFlightPlayerController::SetupInputComponent()
{
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
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;

    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);
}

void AFlightPlayerController::OnShowMouseCompeted(const FInputActionValue& Value)
{   
    bShowMouseCursor = false;
    bEnableClickEvents = false;
    bEnableMouseOverEvents = false;

    FInputModeGameOnly InputMode;
    SetInputMode(InputMode);
}

void AFlightPlayerController::OnToggleListTrigged(const FInputActionValue& Value)
{
    ToggleFlightListUI();
}

void AFlightPlayerController::PerformInteractionTrace()
{
    FVector CamLoc;
    FRotator CamRot;
    GetPlayerViewPoint(CamLoc, CamRot);

    FVector Start = CamLoc;
    FVector End = Start + (CamRot.Vector() * 50000.0f); 

    FHitResult HitResult;
    FCollisionQueryParams Params; 
    Params.AddIgnoredActor(GetPawn()); 

    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult, 
        Start, 
        End,	
        ECC_Visibility, 
        Params 
    );

    // DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.1f, 0, 1.0f);

    if (bHit)
    {
        UInstancedStaticMeshComponent* HitISMC = Cast<UInstancedStaticMeshComponent>(HitResult.GetComponent());

        if (HitISMC)
        {
            int32 InstanceIndex = HitResult.Item;

            if (InstanceIndex != -1)
            {
                if (WaypointInfoWidget)
                {
                    if (!WaypointInfoWidget->IsInViewport())
                    {
                        WaypointInfoWidget->AddToViewport();
                    }

                    if (CachedVisualizer && CachedVisualizer->ParsedGPSPoints.IsValidIndex(InstanceIndex))
                    {
                        // 1. Lấy dữ liệu cơ bản
                        FFlightPoint CurrentPoint = CachedVisualizer->ParsedGPSPoints[InstanceIndex];
                        
                        // 2. Tính toán các thông số phụ (Velocity, Bearing)
                        float Velocity = 0.0f;
                        float Bearing = 0.0f;

                        // Nếu có điểm tiếp theo, tính toán dựa trên điểm tiếp theo
                        if (CachedVisualizer->ParsedGPSPoints.IsValidIndex(InstanceIndex + 1))
                        {
                            FFlightPoint NextPoint = CachedVisualizer->ParsedGPSPoints[InstanceIndex + 1];
                            Velocity = (float)UFlightMathLibrary::ComputeInstanVeclocity(CurrentPoint, NextPoint);
                            Bearing = (float)UFlightMathLibrary::ComputeBearing(CurrentPoint, NextPoint);
                        }
                        // Nếu là điểm cuối cùng, lấy thông số của đoạn trước đó
                        else if (InstanceIndex > 0)
                        {
                            FFlightPoint PrevPoint = CachedVisualizer->ParsedGPSPoints[InstanceIndex - 1];
                            Velocity = (float)UFlightMathLibrary::ComputeInstanVeclocity(PrevPoint, CurrentPoint);
                            Bearing = (float)UFlightMathLibrary::ComputeBearing(PrevPoint, CurrentPoint);
                        }

                        // 3. Chuẩn bị lệnh gọi hàm Blueprint
                        // Format chuỗi lệnh: "UpdateWaypointInfo Lat Lon Alt Index Velocity Bearing Time"
                        // Lưu ý: String phải để trong dấu ngoặc kép nếu có khoảng trắng, nhưng Time ở đây thường là HH:MM:SS nên cẩn thận.
                        // Tốt nhất là truyền Time dưới dạng String bao quanh bởi dấu nháy đơn hoặc xử lý trong BP.
                        
                        FOutputDeviceNull Ar;
                        FString Cmd = FString::Printf(
                            TEXT("UpdateWaypointInfo %f %f %f %d %f %f \"%s\""), 
                            CurrentPoint.Latitude, 
                            CurrentPoint.Longitude, 
                            CurrentPoint.Altitude,
                            InstanceIndex + 1, // Index bắt đầu từ 1 cho thân thiện
                            Velocity,
                            Bearing,
                            *CurrentPoint.Timestamp
                        );
                        
                        WaypointInfoWidget->CallFunctionByNameWithArguments(*Cmd, Ar, nullptr, true);
                    }
                }
                return; 
            }
        }
    }
    
    // Ẩn UI khi nhìn ra ngoài
    if (WaypointInfoWidget && WaypointInfoWidget->IsInViewport())
    {
        WaypointInfoWidget->RemoveFromParent();
    }
}
