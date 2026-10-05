#pragma once

#include "CoreMinimal.h"
#include "FPS/FPSInteractionActor.h"
#include "FPSDoor.generated.h"

class UCurveFloat;
class UTimelineComponent;
class UStaticMeshComponent;
class USceneComponent;
struct FHitResult;

/** 使用浮点曲线驱动的简单交互门。 */
UCLASS(Blueprintable)
class CCSTUDY_API AFPSDoor : public AFPSInteractionActor
{
	GENERATED_BODY()

public:
	AFPSDoor();

	virtual bool CanInteractFromHit(const FHitResult& Hit) const override;

protected:
	virtual void BeginPlay() override;
	virtual void Interact_Implementation(AActor* Interactor) override;

	/** 在派生蓝图中将门轴放到合页位置。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<USceneComponent> DoorPivot;

	/** 会旋转的门网格体，在派生蓝图中指定静态网格体。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	/** 门把手附近的隐藏交互代理，仅用于摄像机射线检测。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Interaction")
	TObjectPtr<UStaticMeshComponent> InteractionMesh;

	/** 曲线输出作为 0（关闭）到 1（打开）的 Alpha。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door|Animation")
	TObjectPtr<UCurveFloat> DoorCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Animation")
	float OpenAngle = 90.0f;

	/** 本地旋转轴，默认使用本地 Z 轴。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door|Animation")
	FVector RotationAxis = FVector::UpVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door|Animation")
	TObjectPtr<UTimelineComponent> OpenDoorTimeline;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Door|Animation")
	bool bIsOpen = false;

	UFUNCTION()
	void UpdateDoorRotation(float Alpha);

	FQuat ClosedRotation;
	FQuat OpenRotation;
};
