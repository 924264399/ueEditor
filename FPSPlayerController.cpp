#include "FPS/FPSPlayerController.h"
#include "FPS/FPSInventoryPanelWidget.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("AFPSPlayerController::BeginPlay"));

	CreateMainHud();
	CreateInventoryMenu();
}

void AFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput =
		Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (ToggleInventoryAction)
		{
			EnhancedInput->BindAction(
				ToggleInventoryAction,
				ETriggerEvent::Started,
				this,
				&AFPSPlayerController::ToggleInventory); //TAB键位绑定这个道具栏的开启关闭
		}
	}
}

void AFPSPlayerController::CreateMainHud()
{
	// 联机时服务器上也有其他玩家的控制器，UI 只给本机玩家创建
	if (!IsLocalController() || !MainHudClass || MainHudRef)
	{
		return;
	}

	MainHudRef = CreateWidget<UUserWidget>(this, MainHudClass); //创建Weight
	if (MainHudRef)
	{
		MainHudRef->AddToViewport();
	}
}

void AFPSPlayerController::CreateInventoryMenu()
{
	// UI 只在本地玩家的 PlayerController 上创建，避免联机时为服务器/其他玩家创建界面。
	if (!IsLocalController() || !InventoryMenuClass || InventoryMenuRef)
	{
		return;
	}

	InventoryMenuRef = CreateWidget<UUserWidget>(this, InventoryMenuClass);
	if (InventoryMenuRef)
	{
		InventoryMenuRef->AddToViewport();
		InventoryPanelRef = Cast<UFPSInventoryPanelWidget>(InventoryMenuRef);
		if (!InventoryPanelRef.IsValid() && InventoryMenuRef->WidgetTree)
		{
			TArray<UWidget*> ChildWidgets;
			InventoryMenuRef->WidgetTree->GetAllWidgets(ChildWidgets);
			for (UWidget* ChildWidget : ChildWidgets)
			{
				if (UFPSInventoryPanelWidget* InventoryPanel = Cast<UFPSInventoryPanelWidget>(ChildWidget))
				{
					InventoryPanelRef = InventoryPanel;
					break;
				}
			}
		}
		if (UFPSInventoryPanelWidget* InventoryPanel = InventoryPanelRef.Get())
		{
			InventoryPanel->BindMenuHost(InventoryMenuRef);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AFPSPlayerController: No FPS inventory panel found in InventoryMenuClass."));
		}
		InventoryMenuRef->SetVisibility(ESlateVisibility::Collapsed);
	}
}


//道具栏状态控制
void AFPSPlayerController::ToggleInventory(const FInputActionValue& /*Value*/)
{
	if (bInventoryOpen)
	{
		CloseInventory();
	}
	else
	{
		OpenInventory();
	}
}


//开启道具栏
void AFPSPlayerController::OpenInventory()
{
	if (!InventoryMenuRef)
	{
		return;
	}

	bInventoryOpen = true;
	InventoryMenuRef->SetVisibility(ESlateVisibility::Visible);

	SetShowMouseCursor(true);
	SetIgnoreMoveInput(true);

	if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn()))
	{
		if (UCharacterMovementComponent* Movement =
			PlayerCharacter->GetCharacterMovement())
		{
			Movement->DisableMovement();
		}
	}

	FInputModeGameAndUI InputMode;
	InputMode.SetWidgetToFocus(InventoryMenuRef->TakeWidget());
	SetInputMode(InputMode);
}


////关闭这个道具栏的可视
void AFPSPlayerController::CloseInventory()
{
	if (!InventoryMenuRef)
	{
		return;
	}

	bInventoryOpen = false;
	if (UFPSInventoryPanelWidget* InventoryPanel = InventoryPanelRef.Get())
	{
		InventoryPanel->CloseDropDownMenu();
	}
	InventoryMenuRef->SetVisibility(ESlateVisibility::Collapsed);

	SetShowMouseCursor(false);
	SetIgnoreMoveInput(false);
	SetInputMode(FInputModeGameOnly());

	if (ACharacter* PlayerCharacter = Cast<ACharacter>(GetPawn()))
	{
		if (UCharacterMovementComponent* Movement =
			PlayerCharacter->GetCharacterMovement())
		{
			Movement->SetMovementMode(MOVE_Walking);
		}
	}
}
