#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "FPSFlickeringLight.generated.h"

class UPointLightComponent;

/** 一个自带点光源的故障闪烁灯。灯光参数在派生蓝图中配置。 */
UCLASS(Blueprintable)
class CCSTUDY_API AFPSFlickeringLight : public AActor
{
	GENERATED_BODY()

public:
	AFPSFlickeringLight();

	/** 开始或重新开始闪烁。 */
	UFUNCTION(BlueprintCallable, Category = "灯光闪烁", meta = (DisplayName = "开始闪烁"))
	void StartFlickering();

	/** 停止闪烁，并恢复到正常强度。 */
	UFUNCTION(BlueprintCallable, Category = "灯光闪烁", meta = (DisplayName = "停止闪烁"))
	void StopFlickering();

protected:
	virtual void BeginPlay() override;

	/** 闪烁灯光组件；颜色、位置和衰减等参数可在蓝图中设置。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "灯光", meta = (DisplayName = "点光源"))
	TObjectPtr<UPointLightComponent> PointLight;

	/** 正常状态下的实际强度；设为 0 时使用 PointLight 初始强度。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "灯光闪烁", meta = (DisplayName = "基准强度", ClampMin = "0.0"))
	float NormalIntensity = 0.0f;

	/** 闪烁时允许的最低倍率，例如 0.3 表示基准强度的 30%。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "灯光闪烁", meta = (DisplayName = "最低闪烁倍率", ClampMin = "0.0"))
	float MinFlickerMultiplier = 0.0f;

	/** 闪烁时允许的最高倍率，例如 1.0 表示基准强度的 100%。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "灯光闪烁", meta = (DisplayName = "最高闪烁倍率", ClampMin = "0.0"))
	float MaxFlickerMultiplier = 1.0f;

	/** 两次强度变化之间的最短时间。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "灯光闪烁", meta = (DisplayName = "最短闪烁间隔", ClampMin = "0.01"))
	float MinFlickerTime = 0.05f;

	/** 两次强度变化之间的最长时间。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "灯光闪烁", meta = (DisplayName = "最长闪烁间隔", ClampMin = "0.01"))
	float MaxFlickerTime = 0.4f;

	/** BeginPlay 时是否自动开始。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "灯光闪烁", meta = (DisplayName = "自动开始闪烁"))
	bool bAutoStart = true;

private:
	void ApplyNextFlicker();
	void ScheduleNextFlicker();

	FTimerHandle FlickerTimerHandle;
	float CachedNormalIntensity = 0.0f;
};
