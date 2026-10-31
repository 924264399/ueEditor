#include "FPS/FPSInventoryComponent.h"

#include "FPS/FPSCharacter.h"
#include "FPS/FPSItemDefinition.h"

UFPSInventoryComponent::UFPSInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFPSInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Capacity = FMath::Max(1, Capacity);
	InventorySlots.SetNum(Capacity);
	OnInventoryChanged.Broadcast();
}

bool UFPSInventoryComponent::AddItem(UFPSItemDefinition* ItemDefinition, int32 Amount)
{
	if (!IsValid(ItemDefinition) || Amount <= 0 || !HasRoomFor(ItemDefinition, Amount))
	{
		return false;
	}

	const int32 MaxStackAmount = FMath::Max(1, ItemDefinition->MaxStackAmount);
	int32 RemainingAmount = Amount;

	// 先补满已有的同类道具堆叠。
	for (FFPSInventorySlot& Slot : InventorySlots)
	{
		if (Slot.IsEmpty() || Slot.ItemDefinition != ItemDefinition || Slot.Amount >= MaxStackAmount)
		{
			continue;
		}

		const int32 AddedAmount = FMath::Min(MaxStackAmount - Slot.Amount, RemainingAmount);
		Slot.Amount += AddedAmount;
		RemainingAmount -= AddedAmount;

		if (RemainingAmount == 0)
		{
			break;
		}
	}

	// 剩余数量按最大堆叠数写入空槽位。
	for (FFPSInventorySlot& Slot : InventorySlots)
	{
		if (RemainingAmount == 0)
		{
			break;
		}

		if (!Slot.IsEmpty())
		{
			continue;
		}

		const int32 AddedAmount = FMath::Min(MaxStackAmount, RemainingAmount);
		Slot.ItemDefinition = ItemDefinition;
		Slot.Amount = AddedAmount;
		RemainingAmount -= AddedAmount;
	}

	OnInventoryChanged.Broadcast();
	return true;
}

bool UFPSInventoryComponent::UseItem(int32 SlotIndex)
{
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		return false;
	}

	const FFPSInventorySlot& SelectedSlot = InventorySlots[SlotIndex];
	UFPSItemDefinition* ItemDefinition = SelectedSlot.ItemDefinition.Get();
	AFPSCharacter* Character = Cast<AFPSCharacter>(GetOwner());
	if (SelectedSlot.IsEmpty() || !IsValid(ItemDefinition) || !ItemDefinition->bCanUse || !IsValid(Character))
	{
		return false;
	}

	// 道具拒绝使用时不扣数量。
	if (!ItemDefinition->Use(Character))
	{
		return false;
	}

	// 使用行为可能改动库存；扣减前重新确认目标槽位。
	if (!InventorySlots.IsValidIndex(SlotIndex))
	{
		return false;
	}
	FFPSInventorySlot& SlotAfterUse = InventorySlots[SlotIndex];
	if (SlotAfterUse.ItemDefinition != ItemDefinition || SlotAfterUse.Amount <= 0)
	{
		return false;
	}

	--SlotAfterUse.Amount;
	if (SlotAfterUse.Amount == 0)
	{
		SlotAfterUse.ItemDefinition = nullptr;
	}
	OnInventoryChanged.Broadcast();
	return true;
}

bool UFPSInventoryComponent::HasRoomFor(const UFPSItemDefinition* ItemDefinition, int32 Amount) const
{
	if (!IsValid(ItemDefinition) || Amount <= 0)
	{
		return false;
	}

	const int32 MaxStackAmount = FMath::Max(1, ItemDefinition->MaxStackAmount);
	int64 AvailableAmount = 0;

	for (const FFPSInventorySlot& Slot : InventorySlots)
	{
		if (Slot.IsEmpty())
		{
			AvailableAmount += MaxStackAmount;
		}
		else if (Slot.ItemDefinition == ItemDefinition)
		{
			AvailableAmount += FMath::Max(0, MaxStackAmount - Slot.Amount);
		}

		if (AvailableAmount >= Amount)
		{
			return true;
		}
	}

	return false;
}
