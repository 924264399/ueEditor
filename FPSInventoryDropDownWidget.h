#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPSInventoryDropDownWidget.generated.h"

class UButton;

UENUM(BlueprintType)
enum class EFPSInventoryMenuAction : uint8
{
	Use UMETA(DisplayName = "使用"),
	Examine UMETA(DisplayName = "检视"),
	Drop UMETA(DisplayName = "丢弃")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnFPSInventoryMenuActionSelected, EFPSInventoryMenuAction, Action);

/** 菜单只报告玩家选择的操作，具体道具逻辑交给库存面板处理。 */
UCLASS()
class CCSTUDY_API UFPSInventoryDropDownWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "库存|菜单", meta = (DisplayName = "选择菜单操作时"))
	FOnFPSInventoryMenuActionSelected OnActionSelected;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget, DisplayName = "使用按钮"))
	TObjectPtr<UButton> UseButton;

	UPROPERTY(meta = (BindWidget, DisplayName = "检视按钮"))
	TObjectPtr<UButton> ExamineButton;

	UPROPERTY(meta = (BindWidget, DisplayName = "丢弃按钮"))
	TObjectPtr<UButton> DropButton;

private:
	UFUNCTION(Category = "库存|菜单", meta = (DisplayName = "点击使用"))
	void HandleUseClicked();

	UFUNCTION(Category = "库存|菜单", meta = (DisplayName = "点击检视"))
	void HandleExamineClicked();

	UFUNCTION(Category = "库存|菜单", meta = (DisplayName = "点击丢弃"))
	void HandleDropClicked();
};
