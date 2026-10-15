#include "FPS/FPSPlayerController.h"
#include "Blueprint/UserWidget.h"
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
				&AFPSPlayerController::ToggleInventory);
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
		InventoryMenuRef->SetVisibility(ESlateVisibility::Collapsed);
	}
}

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

void AFPSPlayerController::CloseInventory()
{
	if (!InventoryMenuRef)
	{
		return;
	}

	bInventoryOpen = false;
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
