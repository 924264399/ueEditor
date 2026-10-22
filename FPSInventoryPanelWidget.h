#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSInventoryPanelWidget.generated.h"

class UUniformGridPanel;
class UFPSInventoryComponent;
class UFPSInventorySlotWidget;

/** 根据库存容量创建槽位控件，并在库存变化时更新显示。 */
UCLASS()
class CCSTUDY_API UFPSInventoryPanelWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 按库存容量和列数重新生成全部槽位。 */
	UFUNCTION(BlueprintCallable, Category = "库存", meta = (DisplayName = "重新生成槽位"))
	void RebuildSlots();

	/** 从库存组件读取数据并更新所有槽位。 */
	UFUNCTION(BlueprintCallable, Category = "库存", meta = (DisplayName = "刷新库存槽位"))
	void RefreshSlots();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** 蓝图子类中的统一网格面板必须命名为 InventoryGrid。 */
	UPROPERTY(meta = (BindWidget, DisplayName = "库存网格面板"))
	TObjectPtr<UUniformGridPanel> InventoryGrid;

	/** 单个槽位使用的控件蓝图类。 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "库存",
		meta = (DisplayName = "槽位控件类"))
	TSubclassOf<UFPSInventorySlotWidget> SlotWidgetClass;

	/** 尚未取得玩家库存时使用的占位槽位数。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "库存",
		meta = (DisplayName = "占位槽位数量", ClampMin = "0"))
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
	TArray<TObjectPtr<UFPSInventorySlotWidget>> SlotWidgets;

private:
	bool TryBindInventory();
	TWeakObjectPtr<UFPSInventoryComponent> BoundInventory;
};
