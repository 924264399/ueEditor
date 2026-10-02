#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

class UCameraComponent;
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

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

protected:
	virtual void BeginPlay() override;
	virtual void NotifyControllerChanged() override;

	void Look(const FInputActionValue& Value);

	
	//EditDefaultsOnly 意味着这个属性只能在蓝图的"类默认值"里改，拖进场景的某个具体角色上不能单独改
	//增强输入把输入拆成了两种资产，都在内容浏览器里创建：InputAction 和 InputMappingContext（输入映射表）
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	//移动的input action
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	//跳跃的input active
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	//视角灵敏度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input",
		meta = (ClampMin = "0.0", UIMin = "0.0", UIMax = "5.0"))
	float LookSensitivity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction",
		meta = (ClampMin = "0.0", UIMin = "0.0"))
	float InteractionDistance = 350.0f; //检测距离变量

	void Move(const FInputActionValue& Value);
	void TryInteract();
	bool TraceForInteraction(FHitResult& OutHit) const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;
};
