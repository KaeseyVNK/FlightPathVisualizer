// Fill out your copyright notice in the Description page of Project Settings.


#include "FlightPathSplineActor.h"
#include "Components/SplineMeshComponent.h"

// Sets default values
AFlightPathSplineActor::AFlightPathSplineActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
	SetRootComponent(SplineComp);

	SplineComp->bDrawDebug = true;

	//Khoi Tao ISMC
	WaypointISMC = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("WaypointISMC"));
	WaypointISMC->SetupAttachment(RootComponent);
	WaypointISMC->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/// Cau hinh cho phep truyen du lieu custom vao shader
	WaypointISMC->NumCustomDataFloats = 1;
}

// Called when the game starts or when spawned
void AFlightPathSplineActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFlightPathSplineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFlightPathSplineActor::BuildSplineFromPoints(const TArray<FVector>& Points)
{
	//if (Points.Num() == 0) {
	//	UE_LOG(LogTemp, Warning, TEXT("BuildSplineFromPoints: No points provided"));
	//	return;
	//}

	//// Xoa tat ca cac diem hien co tren spline
	//SplineComp->ClearSplinePoints(false);

	//// Them cac diem moi vao spline
	//for (int32 i = 0; i < Points.Num(); i++)
	//{
	//	SplineComp->AddSplinePoint(Points[i], ESplineCoordinateSpace::Local, false);
	//}

	//// Cap nhat spline sau khi them tat ca cac diem
	//SplineComp->SetClosedLoop(false);
	//SplineComp->UpdateSpline();

	//UE_LOG(LogTemp, Log, TEXT("BuildSplineFromPoints: Spline built with %d points"), Points.Num());

	if(Points.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("BuildSplineFromPoints: No points provided"));
		return;
	}

	//1. Setup spline
	SplineComp->ClearSplinePoints(false);
	for (const FVector& Point : Points) {

		//them dieu khien moi vao cuoi duong spline hien tai
		SplineComp->AddSplinePoint(Point, ESplineCoordinateSpace::Local, false);
	}

	//cap nhat spline sau khi them tat ca cac diem
	SplineComp->UpdateSpline();

	//2. Setup intanced Mesh ( Waypoint ) 
	WaypointISMC->ClearInstances();

	if (WaypointMesh)
	{
		WaypointISMC->SetStaticMesh(WaypointMesh);
	}

	if (WaypointMaterial)
	{
		WaypointISMC->SetMaterial(0, WaypointMaterial);
	}

	//Tim do cao min/mã de chuan hoa mau sac
	float MinZ = Points[0].Z;
	float MaxZ = Points[0].Z;
	for (const FVector& P : Points)
	{
		if (P.Z < MinZ) MinZ = P.Z;
		if (P.Z > MaxZ) MaxZ = P.Z;
	}
	float RangeZ = (MaxZ - MinZ) > 0 ? (MaxZ - MinZ) : 1.0f;

	// Loop tao instance
	for (const FVector& P : Points)
	{
		// Tao transform cho moi instance
		FTransform InstanceTransform;
		InstanceTransform.SetLocation(P);
		InstanceTransform.SetScale3D(WaypointScale);

		// Them instance vao ISMC
		int32 InstanceIndex = WaypointISMC->AddInstance(InstanceTransform, true);

		// Tinh toan gia tri chuan hoa do cao giua 0 va 1
		float NormalizedHeight = (P.Z - MinZ) / RangeZ;

		
		WaypointISMC->SetCustomDataValue(InstanceIndex, 0, NormalizedHeight, true);
	}

	WaypointISMC->MarkRenderStateDirty();

	UE_LOG(LogTemp, Log, TEXT("BuildSplineFromPoints: Spline & ISMC built with %d points"), Points.Num());
}

void AFlightPathSplineActor::BuildSplineMeshes(UStaticMesh* SplineMesh, float Width)
{

	// Kiem tra neu SplineMesh hop le
	if (!SplineMesh) 
	{
		UE_LOG(LogTemp, Warning, TEXT("SplineMesh is Null "));
		return;
	}

	// Lay so diem tren spline
	const int32 Count = SplineComp->GetNumberOfSplinePoints();

	for(int32 i = 0; i < Count - 1; i++)
	{
		FVector StartPos, EndPos, StartTan, EndTan;
		// Lay vi tri va tiang tai diem i va i+1
		SplineComp->GetLocalLocationAndTangentAtSplinePoint(i, StartPos, StartTan);
		SplineComp->GetLocalLocationAndTangentAtSplinePoint(i + 1, EndPos, EndTan);


		// Tao spline mesh component moi
		USplineMeshComponent* MeshComp = NewObject<USplineMeshComponent>(this);
		MeshComp->SetMobility(EComponentMobility::Movable);
		MeshComp->SetStaticMesh(SplineMesh);

		if (SplineMaterial) 
		{
			MeshComp->SetMaterial(0, SplineMaterial);
		}

		MeshComp->SetStartAndEnd(StartPos, StartTan, EndPos, EndTan);


		float ScaleFactor = Width ? Width : 2.0f;


		// Gan component vao actor
		FVector Scale(Width, Width, Width);
		MeshComp->SetStartScale(FVector2D(ScaleFactor, ScaleFactor));
		MeshComp->SetEndScale(FVector2D(ScaleFactor, ScaleFactor));

		MeshComp->RegisterComponent();
		MeshComp->AttachToComponent(SplineComp, FAttachmentTransformRules::KeepRelativeTransform);
	}

	UE_LOG(LogTemp, Log, TEXT("[SplineActor] Spline meshes created."));


}

