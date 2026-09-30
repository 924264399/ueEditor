#include "FPS/FPSGameInstance.h"

void UFPSGameInstance::Init()
{
	Super::Init();

	UE_LOG(LogTemp, Log, TEXT("UFPSGameInstance::Init"));
}

void UFPSGameInstance::Shutdown()
{
	UE_LOG(LogTemp, Log, TEXT("UFPSGameInstance::Shutdown"));

	Super::Shutdown();
}
