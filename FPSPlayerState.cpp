#include "FPS/FPSPlayerState.h"

void AFPSPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("AFPSPlayerState::BeginPlay"));
}
