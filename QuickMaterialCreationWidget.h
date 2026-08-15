// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityWidget.h"
#include "QuickMaterialCreationWidget.generated.h"

/**
 * 
 */
UCLASS()
class SUPERMANGER_API UQuickMaterialCreationWidget : public UEditorUtilityWidget
{
	GENERATED_BODY()
	
public:
	
	#pragma region QuickMaterialCreationCore
		
		UFUNCTION(BlueprintCallable)
		void CreateMaterialFromSelectedTextures();
		
		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "CreateMaterialFromSelectedTextures") //Category是分组
		bool bCustomMaterialName = true;//用户是否想自己命名材质
		
		UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "CreateMaterialFromSelectedTextures",Meta =(EditCondition = "bCustomMaterialName")) //使用Meta 保证这个值只在布尔值为true才能编辑
		FString MaterialName = TEXT("M_");
		
		
	#pragma endregion  	
		
	
	
private:	
	#pragma region QuickMaterialCreation
		
		
		bool ProcessSelectedData(const TArray<FAssetData>& SelectedDataToProccess, TArray<UTexture2D*>& OutSelectedTexturesArray,FString& OutSelectedTexturePackage);;

		
	#pragma endregion	
	
	
};
