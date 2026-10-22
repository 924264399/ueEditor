#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSInventoryComponent.generated.h"

class UFPSItemDefinition;

/** 库存数组中一个槽位的运行时数据。 */
USTRUCT(BlueprintType)
struct FFPSInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "库存",
		meta = (DisplayName = "道具定义"))
	TObjectPtr<UFPSItemDefinition> ItemDefinition = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "库存",
		meta = (DisplayName = "道具数量"))
	int32 Amount = 0;

	bool IsEmpty() const
	{
		return ItemDefinition == nullptr || Amount <= 0;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFPSInventoryChanged);  //这是一个广播（委托）  用于拾取物体之后更新UI

/** 保存并管理玩家的库存槽位。 */
UCLASS(ClassGroup = (FPS), meta = (BlueprintSpawnableComponent, DisplayName = "库存组件"))
class CCSTUDY_API UFPSInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFPSInventoryComponent();

	/** 完整放入指定数量后返回 true；空间不足时不修改库存。 */
	UFUNCTION(BlueprintCallable, Category = "库存", meta = (DisplayName = "添加道具"))
	bool AddItem(UFPSItemDefinition* ItemDefinition, int32 Amount);

	UFUNCTION(BlueprintPure, Category = "库存", meta = (DisplayName = "获取库存槽位"))
	const TArray<FFPSInventorySlot>& GetInventorySlots() const { return InventorySlots; }

	UFUNCTION(BlueprintPure, Category = "库存", meta = (DisplayName = "获取库存容量"))
	int32 GetCapacity() const { return Capacity; }

	UPROPERTY(BlueprintAssignable, Category = "库存", meta = (DisplayName = "库存变化时"))
	FOnFPSInventoryChanged OnInventoryChanged;  //委托函数

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "库存",
		meta = (DisplayName = "库存容量", ClampMin = "1", UIMin = "1"))
	int32 Capacity = 8;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "库存",
		meta = (DisplayName = "库存槽位"))
	TArray<FFPSInventorySlot> InventorySlots;

private:
	bool HasRoomFor(const UFPSItemDefinition* ItemDefinition, int32 Amount) const;
};
