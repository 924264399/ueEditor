#include "FPS/FPSGameMode.h"
#include "FPS/FPSGameState.h"
#include "FPS/FPSCharacter.h"
#include "FPS/FPSPlayerController.h"
#include "FPS/FPSHUD.h"
#include "FPS/FPSPlayerState.h"

AFPSGameMode::AFPSGameMode()
{
	// Blueprint subclasses can still override these in Class Defaults.
	GameStateClass = AFPSGameState::StaticClass();
	DefaultPawnClass = AFPSCharacter::StaticClass();
	PlayerControllerClass = AFPSPlayerController::StaticClass();
	HUDClass = AFPSHUD::StaticClass();
	PlayerStateClass = AFPSPlayerState::StaticClass();
}

void AFPSGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("AFPSGameMode::BeginPlay"));
}
