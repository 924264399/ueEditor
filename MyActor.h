// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ComboBox.h"
#include "GameFramework/Actor.h"

#include "Components/BoxComponent.h"
#include "Components/SceneCaptureComponent2D.h"

#include "MyActor.generated.h"

UCLASS()
class EDITORSTUDY_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly,Transient,category = "main")  //2D相机  烘焙最常用的 设置为只能看见不能更改 蓝图可读取
	TObjectPtr<USceneCaptureComponent2D> CaptureTerrain;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite,Transient,category = "main") //这个是包围盒
	TObjectPtr<UBoxComponent> BoxComponent; 
	 
	

protected:


public:	
	
};
