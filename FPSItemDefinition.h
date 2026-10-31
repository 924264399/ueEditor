#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FPSItemDefinition.generated.h"

class UTexture2D;
class AFPSCharacter;

/** 描述一种道具的固定资料，运行时库存只保存对此资产的引用。 */
UCLASS(BlueprintType, Blueprintable)
class CCSTUDY_API UFPSItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/** 各道具可覆盖使用行为；返回 true 时库存组件消耗一个。基类不执行具体效果。 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "道具|使用", meta = (DisplayName = "使用道具"))
	bool Use(AFPSCharacter* User) const;
	virtual bool Use_Implementation(AFPSCharacter* User) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "道具",
		meta = (DisplayName = "道具标识"))
	FName ItemId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "道具",
		meta = (DisplayName = "道具名称"))
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "道具",
		meta = (DisplayName = "道具图标"))
	TObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "道具",
		meta = (DisplayName = "最大堆叠数量", ClampMin = "1", UIMin = "1"))
	int32 MaxStackAmount = 1;

	/** 库存组件在调用 Use 前检查这个开关。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "道具|使用",
		meta = (DisplayName = "可以使用"))
	bool bCanUse = false;
};
