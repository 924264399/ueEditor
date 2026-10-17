#pragma once

#include "CoreMinimal.h"
#include "FPS/FPSInteractionActor.h"
#include "FPSPickupActor.generated.h"

class UFPSItemDefinition;
class USphereComponent;
class UStaticMeshComponent;
struct FHitResult;

/** 放置在关卡中、可通过交互键加入库存的拾取物。 */
UCLASS(Blueprintable)
class CCSTUDY_API AFPSPickupActor : public AFPSInteractionActor
{
	GENERATED_BODY()

public:
	AFPSPickupActor();

	virtual bool CanInteractFromHit(const FHitResult& Hit) const override;

protected:
	virtual void Interact_Implementation(AActor* Interactor) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "拾取物",
		meta = (DisplayName = "道具网格体"))
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "拾取物",
		meta = (DisplayName = "交互碰撞"))
	TObjectPtr<USphereComponent> InteractionCollision;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "拾取物",
		meta = (DisplayName = "道具定义"))
	TObjectPtr<UFPSItemDefinition> ItemDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "拾取物",
		meta = (DisplayName = "拾取数量", ClampMin = "1", UIMin = "1"))
	int32 Amount = 1;
};
