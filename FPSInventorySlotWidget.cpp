#include "FPS/FPSInventorySlotWidget.h"

#include "FPS/FPSItemDefinition.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UFPSInventorySlotWidget::UpdateSlot(UFPSItemDefinition* ItemDefinition, int32 Amount)
{
	const bool bHasItem = IsValid(ItemDefinition) && Amount > 0;

	if (SlotImage)
	{
		UTexture2D* Icon = bHasItem ? ItemDefinition->Icon.Get() : nullptr;
		SlotImage->SetBrushFromTexture(Icon);
		SlotImage->SetVisibility(Icon ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
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
