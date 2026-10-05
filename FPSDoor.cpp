#include "FPS/FPSDoor.h"

#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Curves/CurveFloat.h"
#include "Engine/CollisionProfile.h"
#include "Engine/EngineTypes.h"

AFPSDoor::AFPSDoor()
{
	DoorPivot = CreateDefaultSubobject<USceneComponent>(TEXT("DoorPivot"));
	DoorPivot->SetupAttachment(SceneRoot);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(DoorPivot);
	DoorMesh->SetCollisionProfileName(UCollisionProfile::BlockAll_ProfileName);
	// 门体继续阻挡 Pawn，但不应截获交互射线；只有 InteractionMesh 阻挡 Visibility。
	DoorMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	InteractionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("InteractionMesh"));
	InteractionMesh->SetupAttachment(DoorPivot);
	InteractionMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	InteractionMesh->SetHiddenInGame(true);

	OpenDoorTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("OpenDoorTimeline"));
	OpenDoorTimeline->SetLooping(false);
	OpenDoorTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
}

bool AFPSDoor::CanInteractFromHit(const FHitResult& Hit) const
{
	return Hit.GetComponent() == InteractionMesh;
}

void AFPSDoor::BeginPlay()
{
	Super::BeginPlay();

	ClosedRotation = DoorPivot->GetRelativeTransform().GetRotation();
	const FVector Axis = RotationAxis.IsNearlyZero() ? FVector::UpVector : RotationAxis.GetSafeNormal();
	const FQuat OpenOffset(Axis, FMath::DegreesToRadians(OpenAngle));
	// 按门轴的本地坐标应用旋转轴。
	OpenRotation = ClosedRotation * OpenOffset;

	if (!DoorCurve)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has no DoorCurve assigned."), *GetName());
		return;
	}

	FOnTimelineFloat UpdateFunction;
	UpdateFunction.BindUFunction(this, FName(TEXT("UpdateDoorRotation")));
	OpenDoorTimeline->AddInterpFloat(DoorCurve, UpdateFunction);
}


//重写交互接口  按照门的状态  播放timeline
//因为TimeLine 已经绑定了UpdateDoorRotation函数，所以timeline播放时会自动调用UpdateDoorRotation函数
void AFPSDoor::Interact_Implementation(AActor* Interactor)
{
	Super::Interact_Implementation(Interactor);

	if (!DoorCurve)
	{
		return;
	}

	bIsOpen = !bIsOpen;
	if (bIsOpen)
	{
		OpenDoorTimeline->Play();
	}
	else
	{
		OpenDoorTimeline->Reverse();
	}
}


//核心的旋转门的逻辑
void AFPSDoor::UpdateDoorRotation(float Alpha)
{
	const FQuat CurrentRotation = FQuat::Slerp(ClosedRotation, OpenRotation, FMath::Clamp(Alpha, 0.0f, 1.0f));
	DoorPivot->SetRelativeRotation(CurrentRotation);
}
