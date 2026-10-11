#include "FPS/FPSInventoryPanelWidget.h"

#include "Components/UniformGridPanel.h"
#include "GameFramework/PlayerController.h"

void UFPSInventoryPanelWidget::NativeConstruct()    //这个函数之间理解为  一次初始化函数
{
	Super::NativeConstruct();

	RebuildSlots();
}

void UFPSInventoryPanelWidget::RebuildSlots()
{
	if (!InventoryGrid)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("UFPSInventoryPanelWidget: InventoryGrid is not bound. Add a UniformGridPanel named InventoryGrid."));
		return;
	}

	InventoryGrid->ClearChildren();
	SlotWidgets.Reset();

	if (SlotCount <= 0)
	{
		return;
	}

	if (Columns <= 0)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("UFPSInventoryPanelWidget: Columns must be greater than zero."));
		return;
	}

	if (!SlotWidgetClass)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("UFPSInventoryPanelWidget: SlotWidgetClass is not assigned."));
		return;
	}

	InventoryGrid->SetSlotPadding(SlotPadding);

	APlayerController* OwningPlayer = GetOwningPlayer();
	if (!OwningPlayer)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("UFPSInventoryPanelWidget: Cannot create slots without an owning player controller."));
		return;
	}

	SlotWidgets.Reserve(SlotCount);  //先给数组 预留SlotCount个位置

	for (int32 Index = 0; Index < SlotCount; ++Index)
	{
		UUserWidget* NewSlot = CreateWidget<UUserWidget>(OwningPlayer, SlotWidgetClass); //  循环创建 单个槽位的控件
		if (!NewSlot)
		{
			continue;
		}

		const int32 Row = Index / Columns;
		const int32 Column = Index % Columns;  //算这个 行号 列好

		InventoryGrid->AddChildToUniformGrid(NewSlot, Row, Column);   //把刚刚创建出来的 NewSlot 单槽控件，添加到 InventoryGrid 这个统一网格面板的指定行和列中。
		SlotWidgets.Add(NewSlot); ////把创建好的单槽位控件  加入到 SlotWidgets数组中
	}
}
