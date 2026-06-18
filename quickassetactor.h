// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "quickassetactor.generated.h"

/**
 * 
 */
UCLASS() //UE 引擎强制要求的反射宏，写在 C++ 类的最上方。




//SUPERMANGER_API是模块导出宏  让模块外部能访问这个类（比如引擎、其他插件调用你的工具类） 不写的化 引擎找不到它，你的右键菜单直接失效
//Uquickassetactor 强制加U 是UE 官方强制命名规范 U 开头 = 继承自 UObject 的类（所有 UE 功能类都遵守）
//GENERATED_BODY()是UE反射自动生成代码宏，替代了老式的构造函数 / 析构函数。
class SUPERMANGER_API Uquickassetactor : public UAssetActionUtility
{
	GENERATED_BODY() //函数反射宏，和UCLASS()配对使用，标记这是 UE 可识别的函数。

	
public:
	UFUNCTION(CallInEditor) //CallInEditor允许在编辑器中直接调用这个函数； 还有诸如BlueprintCallable：蓝图可调用
	void DuplicateAsset(int32 NumOfDuplicates);
	
	
	
	UFUNCTION(CallInEditor)
	void TestFunction();

	UFUNCTION(CallInEditor)
	void AddPrefix();

	UFUNCTION(CallInEditor)
	void RemoveUnusedAssets();
	

private:
	TMap<UClass*,FString> PrefixMap = 
	{

		{UBlueprint::StaticClass(), TEXT("BP_")}   //StaticClass()是所有Object子类的静态函数，核心作用就是返回 UClass 元数据对象的指针（包含：类名、父类、属性、函数、接口）


	};
	
	void FixUpRedirectors();
	
	
};
 