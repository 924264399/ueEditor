#pragma once

#include "CoreMinimal.h"
#include "FPS/FPSInteractionActor.h"
#include "FPSLightSwitch.generated.h"

class AActor;
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "灯光开关", meta = (DisplayName = "开关模型"))
	TObjectPtr<UStaticMeshComponent> SwitchMesh;

	/** 放置到关卡后指定要控制的 Actor；目标 Actor 需要包含 ULightComponent。 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "灯光开关", meta = (DisplayName = "目标灯光 Actor"))
	TObjectPtr<AActor> TargetLight;
};
