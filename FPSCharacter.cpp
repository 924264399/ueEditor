#include "FPS/FPSCharacter.h"
#include "FPS/FPSInteractable.h"
#include "FPS/FPSInteractionActor.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SpotLightComponent.h"
#include "CollisionQueryParams.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"

AFPSCharacter::AFPSCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(0.f, 0.f, BaseEyeHeight));
	FirstPersonCamera->bUsePawnControlRotation = true;

	// 第一人称手电筒使用长度为 0 的弹簧臂，保留可调节的组件层级并跟随摄像机朝向。
	FlashlightArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("FlashlightArm"));
	FlashlightArm->SetupAttachment(FirstPersonCamera);
	FlashlightArm->TargetArmLength = 0.0f;
	FlashlightArm->bDoCollisionTest = false;

	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetupAttachment(FlashlightArm);
	Flashlight->SetRelativeLocation(FVector(25.0f, 0.0f, 0.0f));
	Flashlight->SetRelativeRotation(FRotator::ZeroRotator);
	Flashlight->SetIntensity(5000.0f);
	Flashlight->SetAttenuationRadius(1500.0f);
	Flashlight->SetInnerConeAngle(20.0f);
	Flashlight->SetOuterConeAngle(35.0f);
	Flashlight->SetCastShadows(true);
	Flashlight->SetVisibility(bFlashlightOn);
}

void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (Flashlight)
	{
		Flashlight->SetVisibility(bFlashlightOn);
	}

	UE_LOG(LogTemp, Log, TEXT("AFPSCharacter::BeginPlay"));
}




/// NotifyControllerChanged 还有 SetupPlayerInputComponent 还有 Look 都是回调函数
///  Pawn 的 Controller 发生变化时由引擎调用。
// 此时 Controller 已经可用，适合给本地玩家添加 Mapping Context。引擎随即调用 NotifyControllerChanged()。 这时我们能拿到控制器，进而拿到玩家（LocalPlayer），把映射表 IMC_Default 加上去。
void AFPSCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (APlayerController* PC = Cast<APlayerController>(Controller))//当前控制这个 Pawn 的 Controller，确实是玩家控制器吗？（还有AI控制器 比如行为树这些控制ai行为的）
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))//有没有本地玩家的 Enhanced Input 子系统？
		{
			if (DefaultMappingContext) //蓝图里的隐射表填了吗？
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);//核心是这句 把引射表加到玩家身上 0是优先级
			}
		}
	}
}


// 本地 Pawn 的 InputComponent 创建完成后由引擎调用。
// 此时适合把 InputAction 绑定到角色函数。
void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//绑定lookaction
		if (LookAction)
		{
			EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Look); //注册回调函数 我们写的look
		}

		//绑定MoveAction
		if (MoveAction)
		{
			EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Move);
		}

		//绑定JumpAction 这里面的回调函数可以直接用ACharacter自带的Jump和StopJumping函数
		if (JumpAction)
		{
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
			EnhancedInput->BindAction(JumpAction, ETriggerEvent::Canceled, this, &ACharacter::StopJumping);
		}

		if (InteractAction)
		{
			EnhancedInput->BindAction(InteractAction, ETriggerEvent::Started, this, &AFPSCharacter::TryInteract);//角色尝试发起交互
		}

		if (FlashlightToggleAction)
		{
			EnhancedInput->BindAction(FlashlightToggleAction, ETriggerEvent::Started, this, &AFPSCharacter::ToggleFlashlight);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("AFPSCharacter: input component is not UEnhancedInputComponent"));
	}
}

void AFPSCharacter::ToggleFlashlight()
{
	bFlashlightOn = !bFlashlightOn;

	if (Flashlight)
	{
		Flashlight->SetVisibility(bFlashlightOn); //先同步灯的状态  默认是关闭 所以开启游戏的时候是关手电筒
	}
}


//为了让移动方向跟随摄像机的水平朝向，通常使用 Controller 的 Yaw
void AFPSCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller)
	{
		const FRotator ControlRotation = Controller->GetControlRotation(); //取到Controller的渲染
		const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);  //只取Yaw 也就是水平旋转角度  其他的Pitch Roll都归零

		const FVector ForwardDirection =
			FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);//根据这个 Yaw，求出世界坐标中的前方方向。 因为ue中X是Forword
		const FVector RightDirection =
			FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);//求Right方向

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}


// 角色尝试发起交互；目标 Actor 的接口函数是 Interact
//这个TryInteract记得绑定到SetupPlayerInputComponent
void AFPSCharacter::TryInteract()
{
	FHitResult HitResult;
	if (!TraceForInteraction(HitResult)) //首先射线检测是否击中可交互物体，如果没有击中就直接返回
	{
		return;
	}

	AActor* HitActor = HitResult.GetActor();  //如果有 就先获取这个Actor的指针
	if (const AFPSInteractionActor* InteractionActor = Cast<AFPSInteractionActor>(HitActor))
	{
		// 特殊交互物可限制必须命中某个组件；普通交互物默认接受自身组件。
		if (!InteractionActor->CanInteractFromHit(HitResult))
		{
			return;
		}
	}

	if (IsValid(HitActor) && HitActor->GetClass()->ImplementsInterface(UFPSInteractable::StaticClass()))  //判断是否存在 + 这个actor是否实现了IFPSInteractable接口
	{
		IFPSInteractable::Execute_Interact(HitActor, this); //直接调用
	}
}



//射线检测函数
bool AFPSCharacter::TraceForInteraction(FHitResult& OutHit) const
{
	UWorld* World = GetWorld();
	if (!World || !FirstPersonCamera)
	{
		return false;
	}

	const FVector Start = FirstPersonCamera->GetComponentLocation();
	const FVector End = Start + FirstPersonCamera->GetForwardVector() * InteractionDistance;

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(this);

	//核心就是调用这个射线检测的函数
	return World->LineTraceSingleByChannel(
		OutHit,
		Start,
		End,
		ECC_Visibility,
		QueryParams);
}


//每一阵 look
//Look() 不直接旋转 Camera
//Look() 旋转的是“玩家控制方向”
//Camera 再跟随这个控制方向
void AFPSCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxis = Value.Get<FVector2D>() * LookSensitivity; //LookSensitivity控制视角灵敏度

	AddControllerYawInput(LookAxis.X);
	AddControllerPitchInput(LookAxis.Y);
}
