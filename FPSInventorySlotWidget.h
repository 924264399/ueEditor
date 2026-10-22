#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSInventorySlotWidget.generated.h"

class UImage;
class UTextBlock;
class UFPSItemDefinition;

/** 显示一个库存槽位的数据，布局由控件蓝图负责。 */
UCLASS()
class CCSTUDY_API UFPSInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "库存", meta = (DisplayName = "更新槽位显示"))
	void UpdateSlot(UFPSItemDefinition* ItemDefinition, int32 Amount);

protected:
	/** 蓝图中的图像控件命名为 SlotImage。 */
	UPROPERTY(meta = (BindWidgetOptional, DisplayName = "道具图像"))
	TObjectPtr<UImage> SlotImage;

	/** 蓝图中的文本控件命名为 AmountText。 */
	UPROPERTY(meta = (BindWidgetOptional, DisplayName = "数量文本"))
	TObjectPtr<UTextBlock> AmountText;

	/** 蓝图中的文本控件命名为 ItemNameText。 */
	UPROPERTY(meta = (BindWidgetOptional, DisplayName = "道具名称文本"))
	TObjectPtr<UTextBlock> ItemNameText;
};
