#include "FPS/FPSGameState.h"

void AFPSGameState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("AFPSGameState::BeginPlay"));
}
