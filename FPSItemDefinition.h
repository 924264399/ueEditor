#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FPSItemDefinition.generated.h"

class UTexture2D;

/** 描述一种道具的固定资料，运行时库存只保存对此资产的引用。 */
UCLASS(BlueprintType)
class CCSTUDY_API UFPSItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
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
};
