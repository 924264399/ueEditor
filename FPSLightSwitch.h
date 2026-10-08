#pragma once

#include "CoreMinimal.h"
#include "FPS/FPSInteractionActor.h"
#include "FPSLightSwitch.generated.h"

class ALight;
class UStaticMeshComponent;

/** 这个类是可直接交互并控制一盏场景灯的开关。 */
UCLASS(Blueprintable)
class CCSTUDY_API AFPSLightSwitch : public AFPSInteractionActor
{
	GENERATED_BODY()

public:
	AFPSLightSwitch();

protected:
	virtual void Interact_Implementation(AActor* Interactor) override;

	/** 开关外观；静态网格体在派生蓝图中指定。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light Switch")
	TObjectPtr<UStaticMeshComponent> SwitchMesh;

	/** 放置到关卡后指定要控制的灯，只允许选择 ALight。 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Light Switch")
	TObjectPtr<ALight> TargetLight;
};
