#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "FPSCharacter.generated.h"

//前置声明  之后在cpp里面 include
//当然你可以直接include
class UCameraComponent;
class UCameraShakeBase;
class USpringArmComponent;
class USpotLightComponent;
class UInputMappingContext;
class UInputAction;
struct FHitResult;
struct FInputActionValue;

UCLASS()
class CCSTUDY_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFPSCharacter();

	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

protected:
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;

	void Look(const FInputActionValue& Value);

	
	//EditDefaultsOnly 意味着这个属性只能在蓝图的"类默认值"里改，拖进场景的某个具体角色上不能单独改
	//增强输入把输入拆成了两种资产，都在内容浏览器里创建：InputAction 和 InputMappingContext（输入映射表）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (DisplayName = "默认输入映射"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (DisplayName = "视角输入动作"))
	TObjectPtr<UInputAction> LookAction;

	//移动的input action
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (DisplayName = "移动输入动作"))
	TObjectPtr<UInputAction> MoveAction;

	//跳跃的input active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (DisplayName = "跳跃输入动作"))
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (DisplayName = "交互输入动作"))
	TObjectPtr<UInputAction> InteractAction;

	// 手电筒开关的 Input Action；资产在蓝图子类的 Class Defaults 中指定。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (DisplayName = "手电筒开关输入动作"))
	TObjectPtr<UInputAction> FlashlightToggleAction;

	// 奔跑的 Input Action；资产在蓝图子类的 Class Defaults 中指定。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (DisplayName = "奔跑输入动作"))
	TObjectPtr<UInputAction> SprintAction;

	// 下蹲的 Input Action；资产在蓝图子类的 Class Defaults 中指定。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input", meta = (DisplayName = "下蹲输入动作"))
	TObjectPtr<UInputAction> CrouchAction;

	//视角灵敏度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input",
		meta = (DisplayName = "视角灵敏度", ClampMin = "0.0", UIMin = "0.0", UIMax = "5.0"))
	float LookSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction",
		meta = (DisplayName = "交互距离", ClampMin = "0.0", UIMin = "0.0"))
	float InteractionDistance = 350.0f; //检测距离变量

	// 角色的普通移动速度和奔跑速度，单位是 cm/s。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement",
		meta = (DisplayName = "行走速度", ClampMin = "0.0", UIMin = "0.0"))
	float WalkSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement",
		meta = (DisplayName = "奔跑速度", ClampMin = "0.0", UIMin = "0.0"))
	float RunSpeed = 600.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement",
		meta = (DisplayName = "下蹲速度", ClampMin = "0.0", UIMin = "0.0"))
	float CrouchSpeed = 150.0f;

	// Camera Shake 资产负责具体的摇晃表现；这里负责根据移动状态选择并播放资产。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Bob",
		meta = (DisplayName = "行走相机摇晃"))
	TSubclassOf<UCameraShakeBase> WalkCameraShake;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Bob",
		meta = (DisplayName = "跑步相机摇晃"))
	TSubclassOf<UCameraShakeBase> RunCameraShake;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Bob",
		meta = (DisplayName = "相机摇晃最小移动速度", ClampMin = "0.0", UIMin = "0.0"))
	float CameraBobMinSpeed = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Bob",
		meta = (DisplayName = "站立摇晃强度", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float IdleCameraBobScale = 0.1f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Movement", meta = (DisplayName = "正在奔跑"))
	bool bIsSprinting = false;

	// 体力参数；速率单位为体力值/秒，Timer 按固定间隔更新。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina",
		meta = (DisplayName = "最大体力", ClampMin = "0.0", UIMin = "0.0"))
	float MaxStamina = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Stamina", meta = (DisplayName = "当前体力"))
	float CurrentStamina = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina",
		meta = (DisplayName = "体力消耗速率", ClampMin = "0.0", UIMin = "0.0"))
	float StaminaDrainRate = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina",
		meta = (DisplayName = "体力恢复速率", ClampMin = "0.0", UIMin = "0.0"))
	float StaminaRecoveryRate = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina",
		meta = (DisplayName = "体力更新时间间隔", ClampMin = "0.01", UIMin = "0.01"))
	float StaminaUpdateInterval = 0.1f;

	void Move(const FInputActionValue& Value);
	void StartSprint();
	void StopSprint();
	void StartCrouch();
	void StopCrouch();
	void ToggleCrouch();
	void UpdateStamina();
	void ApplyMovementSpeed();
	bool CanStartSprint() const;
	void TryInteract();
	void ToggleFlashlight();
	bool TraceForInteraction(FHitResult& OutHit) const;
	void UpdateCameraBob();
	void StopCameraBob();

private:
	enum class ECameraBobState : uint8
	{
		None,
		Idle,
		Walk,
		Run
	};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (DisplayName = "第一人称摄像机", AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight", meta = (DisplayName = "手电筒弹簧臂", AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> FlashlightArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flashlight", meta = (DisplayName = "手电筒", AllowPrivateAccess = "true"))
	TObjectPtr<USpotLightComponent> Flashlight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flashlight",
		meta = (DisplayName = "手电筒开启", AllowPrivateAccess = "true"))
	bool bFlashlightOn = false;

	FTimerHandle StaminaTimerHandle;

	ECameraBobState CameraBobState = ECameraBobState::None;
	UPROPERTY(Transient)
	TObjectPtr<UCameraShakeBase> ActiveCameraShake;
};
