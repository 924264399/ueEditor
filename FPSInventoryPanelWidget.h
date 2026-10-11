#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSInventoryPanelWidget.generated.h"

class UUniformGridPanel;

/** 这个类的作用是 根据槽位数量创建控件，并将它们排列到统一网格中。 */
UCLASS()
class CCSTUDY_API UFPSInventoryPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 使用当前槽位数量和列数重新生成全部槽位。 */
	UFUNCTION(BlueprintCallable, Category = "库存", meta = (DisplayName = "重新生成槽位"))
	void RebuildSlots();

protected:
	virtual void NativeConstruct() override;

	/** 蓝图子类中的统一网格面板必须命名为 InventoryGrid。 */
	UPROPERTY(meta = (BindWidget, DisplayName = "库存网格面板"))
	TObjectPtr<UUniformGridPanel> InventoryGrid;

	/** 单个槽位使用的控件蓝图类。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "库存",
		meta = (DisplayName = "槽位控件类"))
	TSubclassOf<UUserWidget> SlotWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "库存",
		meta = (DisplayName = "槽位数量", ClampMin = "0"))
	int32 SlotCount = 8;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "库存",
		meta = (DisplayName = "网格列数", ClampMin = "1"))
	int32 Columns = 4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "库存",
		meta = (DisplayName = "槽位间距", ToolTip = "每个槽位四周的内边距。"))
	FMargin SlotPadding = FMargin(4.0f);

	/** 保存已生成控件的引用，供后续刷新使用。 */
	UPROPERTY(Transient, BlueprintReadOnly, Category = "库存",
		meta = (DisplayName = "已生成的槽位控件"))
	TArray<TObjectPtr<UUserWidget>> SlotWidgets;
};
