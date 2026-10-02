#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FPSInteractable.generated.h"

class AActor;

UINTERFACE(BlueprintType, Blueprintable)
class CCSTUDY_API UFPSInteractable : public UInterface
{
	GENERATED_BODY()
};

class CCSTUDY_API IFPSInteractable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Interaction")
	void Interact(AActor* Interactor);
};
