#include "FPS/FPSInteractionActor.h"
#include "Components/BoxComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "FPS/FPSCharacter.h"

AFPSInteractionActor::AFPSInteractionActor()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBox"));
	RootComponent = InteractionBox;

	InteractionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AFPSInteractionActor::BeginPlay()
{
	Super::BeginPlay();

	// 这是教程中“交互物体主动获取第 0 个玩家”的写法示例。
	PlayerRef = Cast<AFPSCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}


//重回 Interact 接口函数
//Interactor 就是按下交互按钮的 玩家对象
void AFPSInteractionActor::Interact_Implementation(AActor* Interactor)
{
	// 交互发生时传入的 Interactor 才是真正发起这次交互的玩家。
	if (AFPSCharacter* InteractingCharacter = Cast<AFPSCharacter>(Interactor)) //把交互对象转化为具体的角色类型
	{
		PlayerRef = InteractingCharacter;
	}

	const FString Message = FString::Printf(
		TEXT("%s::Interact called by %s"),
		*GetNameSafe(this),
		*GetNameSafe(PlayerRef));

	UE_LOG(LogTemp, Log, TEXT("%s"), *Message);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Green,
			Message);
	}
}
