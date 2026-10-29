#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Styling/SlateBrush.h"
#include "FPSInventorySlotWidget.generated.h"

class UImage;
class UButton;
class UTextBlock;
class UFPSItemDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFPSInventorySlotReleased, int32, SlotIndex);

/** 显示一个库存槽位的数据，布局由控件蓝图负责。 */
UCLASS()
class CCSTUDY_API UFPSInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSlotIndex(int32 InSlotIndex) { SlotIndex = InSlotIndex; }

	UFUNCTION(BlueprintCallable, Category = "库存", meta = (DisplayName = "更新槽位显示"))
	void UpdateSlot(UFPSItemDefinition* ItemDefinition, int32 Amount);

	UPROPERTY(BlueprintAssignable, Category = "库存", meta = (DisplayName = "槽位按钮松开时"))
	FOnFPSInventorySlotReleased OnSlotReleased;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** 空槽仍显示按钮外观，但不响应鼠标悬停或点击。 */
	UPROPERTY(meta = (BindWidget, DisplayName = "槽位按钮"))
	TObjectPtr<UButton> SlotButton;

	/** 蓝图中的图像控件命名为 SlotImage。 */
	UPROPERTY(meta = (BindWidgetOptional, DisplayName = "道具图像"))
	TObjectPtr<UImage> SlotImage;

	/** 蓝图中的文本控件命名为 AmountText。 */
	UPROPERTY(meta = (BindWidgetOptional, DisplayName = "数量文本"))
	TObjectPtr<UTextBlock> AmountText;

	/** 蓝图中的文本控件命名为 ItemNameText。 */
	UPROPERTY(meta = (BindWidgetOptional, DisplayName = "道具名称文本"))
	TObjectPtr<UTextBlock> ItemNameText;

private:
	UFUNCTION(Category = "库存", meta = (DisplayName = "处理槽位按钮松开"))
	void HandleSlotButtonReleased();

	int32 SlotIndex = INDEX_NONE;

	/** 首次刷新时保存蓝图中 SlotImage 设置的空槽图案。 */
	UPROPERTY(Transient)
	FSlateBrush EmptySlotBrush;

	bool bHasCachedEmptyBrush = false;
};
