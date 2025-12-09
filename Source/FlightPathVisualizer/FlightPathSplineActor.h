// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "FlightPathSplineActor.generated.h"

UCLASS()
class FLIGHTPATHVISUALIZER_API AFlightPathSplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFlightPathSplineActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// spline su dung de ve duong bay
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Spline")
	USplineComponent* SplineComp;

	//Component de ve hang loat waypoint toi uu
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Visualization")
	UInstancedStaticMeshComponent* WaypointISMC;

	//Mesh hien thi cho moi diem ( vd: Sphere )
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualization")
	UStaticMesh* WaypointMesh;

	//Vat lieu hien thi cho moi diem
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	UMaterialInterface* WaypointMaterial;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	UMaterialInterface* SplineMaterial;


	//==============================================
	// Ham nhan du lieu va dung spline
	//==============================================
	UFUNCTION(BlueprintCallable, Category = "Flight Path")
	void BuildSplineFromPoints(const TArray<FVector>& Points);


	//tao sline voi mesh 
	UFUNCTION(BlueprintCallable, Category = "Flight Path")
	void BuildSplineMeshes(UStaticMesh* SplineMesh, float Width = 20.0f);



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	FVector WaypointScale = FVector(1.0f);
};
