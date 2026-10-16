#include "FPS/FPSPickupActor.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/Engine.h"
#include "Engine/EngineTypes.h"
#include "FPS/FPSCharacter.h"
#include "FPS/FPSInventoryComponent.h"
#include "FPS/FPSItemDefinition.h"

AFPSPickupActor::AFPSPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(SceneRoot);
	ItemMesh->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	ItemMesh->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);

	InteractionCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));
	InteractionCollision->SetupAttachment(SceneRoot);
	InteractionCollision->InitSphereRadius(60.0f);
	InteractionCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCollision->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

bool AFPSPickupActor::CanInteractFromHit(const FHitResult& Hit) const
{
	return Hit.GetComponent() == InteractionCollision;
}

void AFPSPickupActor::Interact_Implementation(AActor* Interactor)
{
	AFPSCharacter* Character = Cast<AFPSCharacter>(Interactor);
	if (!Character || !ItemDefinition || Amount <= 0)
	{
		return;
	}

	UFPSInventoryComponent* Inventory = Character->GetInventoryComponent();
	if (!Inventory)
	{
		return;
	}

	const FText ItemDisplayName = ItemDefinition->DisplayName.IsEmpty()
		? FText::FromName(ItemDefinition->ItemId)
		: ItemDefinition->DisplayName;

	if (!Inventory->AddItem(ItemDefinition, Amount))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Red,
				FString::Printf(TEXT("库存空间不足：%s × %d"), *ItemDisplayName.ToString(), Amount));
		}
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Green,
			FString::Printf(TEXT("获得：%s × %d"), *ItemDisplayName.ToString(), Amount));
	}

	UE_LOG(LogTemp, Log, TEXT("Picked up %s x%d"), *GetNameSafe(ItemDefinition), Amount);
	Destroy();
}
