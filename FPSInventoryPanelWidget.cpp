#include "FPS/FPSInventoryPanelWidget.h"

#include "FPS/FPSCharacter.h"
#include "FPS/FPSInventoryComponent.h"
#include "FPS/FPSInventorySlotWidget.h"
#include "Components/UniformGridPanel.h"
#include "GameFramework/PlayerController.h"

void UFPSInventoryPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
	TryBindInventory();
	//TryBindInventory这个函数：
	//取得拥有这个 UI 的 PlayerController 当前控制的 Pawn。
	//把 Pawn 转为 AFPSCharacter。
	//取得角色的 UFPSInventoryComponent，监听它的事件。
	
	
	RebuildSlots();
}

void UFPSInventoryPanelWidget::NativeDestruct()
{
	if (UFPSInventoryComponent* Inventory = BoundInventory.Get())
	{
		Inventory->OnInventoryChanged.RemoveDynamic(this, &UFPSInventoryPanelWidget::RefreshSlots);
	}
	BoundInventory.Reset();

	Super::NativeDestruct();
}

void UFPSInventoryPanelWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 菜单可能先于玩家 Pawn 创建；取得库存后只需重建一次。
	if (TryBindInventory())
	{
		RebuildSlots();
	}
}

bool UFPSInventoryPanelWidget::TryBindInventory()
{
	const AFPSCharacter* Character = Cast<AFPSCharacter>(GetOwningPlayerPawn());
	UFPSInventoryComponent* NewInventory = Character ? Character->GetInventoryComponent() : nullptr;
	if (BoundInventory.Get() == NewInventory)
	{
		return false;
	}

	if (UFPSInventoryComponent* OldInventory = BoundInventory.Get())
	{
		OldInventory->OnInventoryChanged.RemoveDynamic(this, &UFPSInventoryPanelWidget::RefreshSlots);
	}

	BoundInventory = NewInventory;
	if (NewInventory)
	{
		NewInventory->OnInventoryChanged.AddUniqueDynamic(this, &UFPSInventoryPanelWidget::RefreshSlots); //库存发生变化时 自动调用这个RefreshSlots函数
	}
	return true;
}

void UFPSInventoryPanelWidget::RebuildSlots()
{
	if (!InventoryGrid)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFPSInventoryPanelWidget: InventoryGrid is not bound."));
		return;
	}

	InventoryGrid->ClearChildren();
	SlotWidgets.Reset();

	const UFPSInventoryComponent* Inventory = BoundInventory.Get();
	const int32 DesiredSlotCount = Inventory ? Inventory->GetCapacity() : FMath::Max(0, SlotCount);
	if (DesiredSlotCount == 0)
	{
		return;
	}

	if (Columns <= 0 || !SlotWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFPSInventoryPanelWidget: Set Columns and SlotWidgetClass."));
		return;
	}

	APlayerController* OwningPlayer = GetOwningPlayer();
	if (!OwningPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFPSInventoryPanelWidget: Owning player is missing."));
		return;
	}

	InventoryGrid->SetSlotPadding(SlotPadding);
	SlotWidgets.SetNum(DesiredSlotCount);
	for (int32 Index = 0; Index < DesiredSlotCount; ++Index)
	{
		UFPSInventorySlotWidget* NewSlot = CreateWidget<UFPSInventorySlotWidget>(OwningPlayer, SlotWidgetClass);
		if (!NewSlot)
		{
			continue;
		}

		InventoryGrid->AddChildToUniformGrid(NewSlot, Index / Columns, Index % Columns);
		SlotWidgets[Index] = NewSlot;
		NewSlot->UpdateSlot(nullptr, 0);
	}

	RefreshSlots();
}



//广播发生后，调用这个函数，每个 UI 槽位读取同下标的库存数据，然后调用 UpdateSlot()
void UFPSInventoryPanelWidget::RefreshSlots()
{
	const UFPSInventoryComponent* Inventory = BoundInventory.Get();
	if (!Inventory)
	{
		return;
	}

	if (SlotWidgets.Num() != Inventory->GetCapacity())
	{
		RebuildSlots();
		return;
	}

	const TArray<FFPSInventorySlot>& InventorySlots = Inventory->GetInventorySlots();
	for (int32 Index = 0; Index < SlotWidgets.Num(); ++Index)
	{
		UFPSInventorySlotWidget* SlotWidget = SlotWidgets[Index];
		if (!SlotWidget)
		{
			continue;
		}

		if (InventorySlots.IsValidIndex(Index))
		{
			const FFPSInventorySlot& InventorySlot = InventorySlots[Index];
			SlotWidget->UpdateSlot(InventorySlot.ItemDefinition.Get(), InventorySlot.Amount);//UpdateSlot是单个槽位更新画面
		}
		else
		{
			SlotWidget->UpdateSlot(nullptr, 0);  //如果是空槽  图标数量名称全部隐藏
		}
	}
}
