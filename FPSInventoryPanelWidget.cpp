#include "FPS/FPSInventoryPanelWidget.h"

#include "FPS/FPSCharacter.h"
#include "FPS/FPSInventoryComponent.h"
#include "FPS/FPSInventoryDropDownWidget.h"
#include "FPS/FPSInventorySlotWidget.h"
#include "FPS/FPSItemDefinition.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/UniformGridPanel.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"

void UFPSInventoryPanelWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CloseDropDownMenu();

	TryBindInventory();
	//TryBindInventory这个函数：
	//取得拥有这个 UI 的 PlayerController 当前控制的 Pawn。
	//把 Pawn 转为 AFPSCharacter。
	//取得角色的 UFPSInventoryComponent，监听它的事件。
	
	
	RebuildSlots();
}

void UFPSInventoryPanelWidget::NativeDestruct()
{
	CloseDropDownMenu();
	if (InventoryDropDown)
	{
		InventoryDropDown->OnActionSelected.RemoveDynamic(this, &UFPSInventoryPanelWidget::HandleMenuAction);
	}
	if (DismissMenuButton)
	{
		DismissMenuButton->OnClicked.RemoveDynamic(this, &UFPSInventoryPanelWidget::HandleDismissMenuClicked); //点击去菜单栏的按钮
	}
	for (UFPSInventorySlotWidget* SlotWidget : SlotWidgets)
	{
		if (SlotWidget)
		{
			SlotWidget->OnSlotReleased.RemoveDynamic(this, &UFPSInventoryPanelWidget::HandleSlotReleased);
		} //OnSlotReleased是委托 是单个槽位那边创建的 当点击的时候  进行广播  我们这个回应这个委托 并且调用回调函数
	}

	if (UFPSInventoryComponent* Inventory = BoundInventory.Get())
	{
		Inventory->OnInventoryChanged.RemoveDynamic(this, &UFPSInventoryPanelWidget::RefreshSlots);
	}
	BoundInventory.Reset();

	Super::NativeDestruct();
}

void UFPSInventoryPanelWidget::BindMenuHost(UUserWidget* MenuHost)
{
	if (InventoryDropDown)
	{
		InventoryDropDown->OnActionSelected.RemoveDynamic(this, &UFPSInventoryPanelWidget::HandleMenuAction);
	}
	if (DismissMenuButton)
	{
		DismissMenuButton->OnClicked.RemoveDynamic(this, &UFPSInventoryPanelWidget::HandleDismissMenuClicked);
	}

	if (MenuHost && MenuHost != this && MenuHost->WidgetTree)
	{
		InventoryCanvas = Cast<UCanvasPanel>(MenuHost->WidgetTree->RootWidget);
		InventoryDropDown = Cast<UFPSInventoryDropDownWidget>(MenuHost->GetWidgetFromName(TEXT("InventoryDropDown")));
		DismissMenuButton = Cast<UButton>(MenuHost->GetWidgetFromName(TEXT("DismissMenuButton")));
		if (!InventoryDropDown)
		{
			TArray<UWidget*> HostWidgets;
			MenuHost->WidgetTree->GetAllWidgets(HostWidgets);
			for (UWidget* HostWidget : HostWidgets)
			{
				if (UFPSInventoryDropDownWidget* FoundDropDown = Cast<UFPSInventoryDropDownWidget>(HostWidget))
				{
					InventoryDropDown = FoundDropDown;
					break;
				}
			}
		}
		if (!InventoryCanvas || !InventoryDropDown || !DismissMenuButton)
		{
			UE_LOG(LogTemp, Warning, TEXT("UFPSInventoryPanelWidget: Menu host %s needs a CanvasPanel root, an FPSInventoryDropDownWidget and a DismissMenuButton."), *MenuHost->GetName());
		}
	}

	CloseDropDownMenu();
	if (InventoryDropDown)
	{
		InventoryDropDown->OnActionSelected.AddUniqueDynamic(this, &UFPSInventoryPanelWidget::HandleMenuAction);
	}
	if (DismissMenuButton)
	{
		DismissMenuButton->SetRenderOpacity(0.0f);
		DismissMenuButton->OnClicked.AddUniqueDynamic(this, &UFPSInventoryPanelWidget::HandleDismissMenuClicked);
	}
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
	CloseDropDownMenu();
	if (!InventoryGrid)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFPSInventoryPanelWidget: InventoryGrid is not bound."));
		return;
	}

	for (UFPSInventorySlotWidget* SlotWidget : SlotWidgets)
	{
		if (SlotWidget)
		{
			SlotWidget->OnSlotReleased.RemoveDynamic(this, &UFPSInventoryPanelWidget::HandleSlotReleased);
		}
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
		NewSlot->SetSlotIndex(Index);
		NewSlot->OnSlotReleased.AddUniqueDynamic(this, &UFPSInventoryPanelWidget::HandleSlotReleased);
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
	if (ActiveMenuSlotIndex != INDEX_NONE &&
		(!InventorySlots.IsValidIndex(ActiveMenuSlotIndex) || InventorySlots[ActiveMenuSlotIndex].IsEmpty()))
	{
		CloseDropDownMenu();
	}
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

void UFPSInventoryPanelWidget::HandleSlotReleased(int32 SlotIndex)
{
	OpenDropDownMenu(SlotIndex);
}




// OpenDropDownMenu函数  确认点的是有道具的槽位
// → 把菜单放到该槽位旁边
// → 显示菜单和“点击外侧关闭”的透明按钮
void UFPSInventoryPanelWidget::OpenDropDownMenu(int32 SlotIndex)
{
	const UFPSInventoryComponent* Inventory = BoundInventory.Get();
	if (!Inventory || !Inventory->GetInventorySlots().IsValidIndex(SlotIndex) ||
		Inventory->GetInventorySlots()[SlotIndex].IsEmpty() || !SlotWidgets.IsValidIndex(SlotIndex) ||
		!SlotWidgets[SlotIndex])
	{
		CloseDropDownMenu();
		return;
	}

	if (!InventoryCanvas || !InventoryDropDown || !DismissMenuButton)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFPSInventoryPanelWidget: Set InventoryCanvas, InventoryDropDown and DismissMenuButton in the widget blueprint."));
		return;
	}

	UCanvasPanelSlot* MenuCanvasSlot = Cast<UCanvasPanelSlot>(InventoryDropDown->Slot);
	UCanvasPanelSlot* DismissCanvasSlot = Cast<UCanvasPanelSlot>(DismissMenuButton->Slot);
	if (!MenuCanvasSlot || !DismissCanvasSlot || InventoryDropDown->GetParent() != InventoryCanvas ||
		DismissMenuButton->GetParent() != InventoryCanvas)
	{
		UE_LOG(LogTemp, Warning, TEXT("UFPSInventoryPanelWidget: InventoryDropDown and DismissMenuButton must be direct children of InventoryCanvas."));
		return;
	}

	DismissCanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f, 1.0f, 1.0f));
	DismissCanvasSlot->SetOffsets(FMargin(0.0f));
	DismissCanvasSlot->SetAlignment(FVector2D::ZeroVector);
	DismissCanvasSlot->SetZOrder(1);
	MenuCanvasSlot->SetZOrder(2);

	ActiveMenuSlotIndex = SlotIndex;
	DismissMenuButton->SetVisibility(ESlateVisibility::Visible);
	InventoryDropDown->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	InventoryCanvas->ForceLayoutPrepass();

	const FGeometry& SlotGeometry = SlotWidgets[SlotIndex]->GetCachedGeometry();
	const FGeometry& CanvasGeometry = InventoryCanvas->GetCachedGeometry();
	const FVector2D SlotTopRight = SlotGeometry.LocalToAbsolute(FVector2D(SlotGeometry.GetLocalSize().X, 0.0f));
	FVector2D MenuPosition = CanvasGeometry.AbsoluteToLocal(SlotTopRight);
	const FVector2D CanvasSize = CanvasGeometry.GetLocalSize();
	const FVector2D MenuSize = InventoryDropDown->GetDesiredSize();
	MenuPosition.X = FMath::Clamp(MenuPosition.X, 0.0f, FMath::Max(0.0f, CanvasSize.X - MenuSize.X));
	MenuPosition.Y = FMath::Clamp(MenuPosition.Y, 0.0f, FMath::Max(0.0f, CanvasSize.Y - MenuSize.Y)); //算位置

	MenuCanvasSlot->SetAnchors(FAnchors(0.0f, 0.0f));
	MenuCanvasSlot->SetAlignment(FVector2D::ZeroVector);
	MenuCanvasSlot->SetAutoSize(true);
	MenuCanvasSlot->SetPosition(MenuPosition); //移动下拉菜单的位置
}

void UFPSInventoryPanelWidget::CloseDropDownMenu()
{
	ActiveMenuSlotIndex = INDEX_NONE;
	if (InventoryDropDown)
	{
		InventoryDropDown->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (DismissMenuButton)
	{
		DismissMenuButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UFPSInventoryPanelWidget::HandleDismissMenuClicked()
{
	CloseDropDownMenu();
}

void UFPSInventoryPanelWidget::HandleMenuAction(EFPSInventoryMenuAction Action)
{
	const int32 SlotIndex = ActiveMenuSlotIndex;
	CloseDropDownMenu();

	UFPSInventoryComponent* Inventory = BoundInventory.Get();
	if (!Inventory || !Inventory->GetInventorySlots().IsValidIndex(SlotIndex))
	{
		return;
	}
	if (Action == EFPSInventoryMenuAction::Use)
	{
		Inventory->UseItem(SlotIndex);
		return;
	}
	const FFPSInventorySlot& InventorySlot = Inventory->GetInventorySlots()[SlotIndex];
	if (InventorySlot.IsEmpty() || !IsValid(InventorySlot.ItemDefinition.Get()))
	{
		return;
	}

	const TCHAR* ActionText = TEXT("未知操作");
	switch (Action)
	{
	case EFPSInventoryMenuAction::Examine: ActionText = TEXT("检视"); break;
	case EFPSInventoryMenuAction::Drop: ActionText = TEXT("丢弃"); break;
	}

	UKismetSystemLibrary::PrintString(this,
		FString::Printf(TEXT("%s：%s（槽位 %d）"), ActionText, *InventorySlot.ItemDefinition->DisplayName.ToString(), SlotIndex),
		true, true, FLinearColor::White, 2.0f);
}
