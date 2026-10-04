#pragma once

#include "CoreMinimal.h"
#include "FPS/FPSInteractable.h"
#include "GameFramework/Actor.h"
#include "FPSInteractionActor.generated.h"

class AFPSCharacter;
class UBoxComponent;

UCLASS(Blueprintable)
class CCSTUDY_API AFPSInteractionActor : public AActor, public IFPSInteractable
{
	GENERATED_BODY()

public:
	AFPSInteractionActor();

protected:
	virtual void BeginPlay() override;

	virtual void Interact_Implementation(AActor* Interactor) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<UBoxComponent> InteractionBox;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<AFPSCharacter> PlayerRef;
};
