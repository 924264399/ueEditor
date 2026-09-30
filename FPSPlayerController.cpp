#include "FPS/FPSPlayerController.h"

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("AFPSPlayerController::BeginPlay"));
}
