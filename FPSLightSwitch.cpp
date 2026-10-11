#include "FPS/FPSLightSwitch.h"

#include "Components/LightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"

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

	ULightComponent* LightComponent = TargetLight->FindComponentByClass<ULightComponent>();
	if (!IsValid(LightComponent))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("%s target %s has no valid LightComponent."),
			*GetName(),
			*TargetLight->GetName());
		return;
	}

	LightComponent->SetVisibility(!LightComponent->IsVisible());
}
