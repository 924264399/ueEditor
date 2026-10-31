#include "FPS/FPSInventoryDropDownWidget.h"

#include "Components/Button.h"

void UFPSInventoryDropDownWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (UseButton)
	{
		UseButton->OnClicked.AddUniqueDynamic(this, &UFPSInventoryDropDownWidget::HandleUseClicked);
	}
	if (ExamineButton)
	{
		ExamineButton->OnClicked.AddUniqueDynamic(this, &UFPSInventoryDropDownWidget::HandleExamineClicked);
	}
	if (DropButton)
	{
		DropButton->OnClicked.AddUniqueDynamic(this, &UFPSInventoryDropDownWidget::HandleDropClicked);
	}
}

void UFPSInventoryDropDownWidget::NativeDestruct()
{
	if (UseButton)
	{
		UseButton->OnClicked.RemoveDynamic(this, &UFPSInventoryDropDownWidget::HandleUseClicked);
	}
	if (ExamineButton)
	{
		ExamineButton->OnClicked.RemoveDynamic(this, &UFPSInventoryDropDownWidget::HandleExamineClicked);
	}
	if (DropButton)
	{
		DropButton->OnClicked.RemoveDynamic(this, &UFPSInventoryDropDownWidget::HandleDropClicked);
	}
	Super::NativeDestruct();
}



// 三个按钮  三个功能的回调函数
void UFPSInventoryDropDownWidget::HandleUseClicked()
{
	OnActionSelected.Broadcast(EFPSInventoryMenuAction::Use);
}

void UFPSInventoryDropDownWidget::HandleExamineClicked()
{
	OnActionSelected.Broadcast(EFPSInventoryMenuAction::Examine);
}

void UFPSInventoryDropDownWidget::HandleDropClicked()
{
	OnActionSelected.Broadcast(EFPSInventoryMenuAction::Drop);
}
