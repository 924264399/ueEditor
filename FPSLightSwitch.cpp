#include "FPS/FPSLightSwitch.h"

#include "Components/LightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Light.h"

AFPSLightSwitch::AFPSLightSwitch()
{
	SwitchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwitchMesh"));
	SwitchMesh->SetupAttachment(SceneRoot);
}

void AFPSLightSwitch::Interact_Implementation(AActor* Interactor)
{
	Super::Interact_Implementation(Interactor);

	if (!IsValid(TargetLight))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no TargetLight assigned."), *GetName());
		return;
	}

	ULightComponent* LightComponent = TargetLight->GetLightComponent();
	if (!IsValid(LightComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no valid light component."), *GetName());
		return;
	}

	LightComponent->SetVisibility(!LightComponent->IsVisible());
}
