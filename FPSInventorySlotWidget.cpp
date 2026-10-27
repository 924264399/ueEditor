#include "FPS/FPSInventorySlotWidget.h"

#include "FPS/FPSItemDefinition.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UFPSInventorySlotWidget::UpdateSlot(UFPSItemDefinition* ItemDefinition, int32 Amount)
{
	const bool bHasItem = IsValid(ItemDefinition) && Amount > 0;
	if (SlotButton)
	{
		SlotButton->SetVisibility(bHasItem
			? ESlateVisibility::Visible
			: ESlateVisibility::HitTestInvisible);//基于道具栏是否有物品 来设置是否可见     HitTestInvisible可见但实不接受鼠标
		                                          // 这样的化   没有物体的时候就不会点亮SlotButton这个按钮
	}

	if (SlotImage)
	{
		if (!bHasCachedEmptyBrush)
		{
			EmptySlotBrush = SlotImage->GetBrush();
			bHasCachedEmptyBrush = true;
		}

		UTexture2D* Icon = bHasItem ? ItemDefinition->Icon.Get() : nullptr;
		if (Icon)
		{
			SlotImage->SetBrushFromTexture(Icon);
		}
		else
		{
			SlotImage->SetBrush(EmptySlotBrush);
		}
		SlotImage->SetVisibility(ESlateVisibility::Visible);
	}

	if (AmountText)
	{
		AmountText->SetText(bHasItem ? FText::AsNumber(Amount) : FText::GetEmpty());
		AmountText->SetVisibility(bHasItem ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (ItemNameText)
	{
		ItemNameText->SetText(bHasItem ? ItemDefinition->DisplayName : FText::GetEmpty());
		ItemNameText->SetVisibility(bHasItem ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}
