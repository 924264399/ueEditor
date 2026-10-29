#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

class UUserWidget;
class UFPSInventoryPanelWidget;
class UInputAction;
struct FInputActionValue;

UCLASS()
class CCSTUDY_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UUserWidget* GetMainHud() const { return MainHudRef; }
	UUserWidget* GetInventoryMenu() const { return InventoryMenuRef; }

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	void CreateMainHud();
	void CreateInventoryMenu();
	void ToggleInventory(const FInputActionValue& Value);
	void OpenInventory();
	void CloseInventory();

	// 在蓝图里指定常驻 HUD。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "界面",
		meta = (DisplayName = "主 HUD 控件类"))
	TSubclassOf<UUserWidget> MainHudClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "界面",
		meta = (DisplayName = "主 HUD 控件引用"))
	TObjectPtr<UUserWidget> MainHudRef;

	// 独立的工具/库存菜单。它只在需要时显示，内容由 UMG 蓝图负责组装。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "界面",
		meta = (DisplayName = "工具菜单控件类"))
	TSubclassOf<UUserWidget> InventoryMenuClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "界面",
		meta = (DisplayName = "工具菜单控件引用"))
	TObjectPtr<UUserWidget> InventoryMenuRef;
	TWeakObjectPtr<UFPSInventoryPanelWidget> InventoryPanelRef;

	// 这个 Input Action 应在蓝图子类的 Class Defaults 中指定，并在输入映射中绑定按键。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "输入",
		meta = (DisplayName = "工具菜单切换输入动作"))
	TObjectPtr<UInputAction> ToggleInventoryAction;

	bool bInventoryOpen = false;
};
