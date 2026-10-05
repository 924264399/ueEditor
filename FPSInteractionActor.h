#pragma once

#include "CoreMinimal.h"
#include "FPS/FPSInteractable.h"
#include "GameFramework/Actor.h"
#include "FPSInteractionActor.generated.h"

class AFPSCharacter;
class USceneComponent;
struct FHitResult;

UCLASS(Blueprintable)
class CCSTUDY_API AFPSInteractionActor : public AActor, public IFPSInteractable
{
	GENERATED_BODY()

public:
	AFPSInteractionActor();

	/** 默认接受命中该 Actor 的任意组件，特殊交互物可覆盖此判断。 */
	virtual bool CanInteractFromHit(const FHitResult& Hit) const;

protected:
	virtual void BeginPlay() override;

	virtual void Interact_Implementation(AActor* Interactor) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Interaction")
	TObjectPtr<AFPSCharacter> PlayerRef;
};
