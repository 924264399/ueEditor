// Fill out your copyright notice in the Description page of Project Settings.


#include "quickassetactor/QuickMaterialCreationWidget.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"

#pragma region QuickMaterialCreationCore

	void UQuickMaterialCreationWidget::CreateMaterialFromSelectedTextures()
	{
		if (bCustomMaterialName)
		{
			
			if (MaterialName.IsEmpty() || MaterialName.Equals(TEXT("M_")))
			{
				DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("Please enter a valid material name."));
				return;
			}
			
		}
		
		
		TArray<FAssetData> SelectedAssetDatas = UEditorUtilityLibrary::GetSelectedAssetData();
		TArray<UTexture2D*> SelectTexture2Ds;
		FString SelectedTextureFolderPath;
		
		
		if (!ProcessSelectedData(SelectedAssetDatas,SelectTexture2Ds,SelectedTextureFolderPath)) return;
		
		DebugHeader::Print(SelectedTextureFolderPath, FColor::Green);
		
		
	}



#pragma endregion


#pragma region QuickMaterialCreation

bool UQuickMaterialCreationWidget::ProcessSelectedData(const TArray<FAssetData>& SelectedDataToProccess,
	TArray<UTexture2D*>& OutSelectedTexturesArray, FString& OutSelectedTexturePackage)
	{
		
		if (SelectedDataToProccess.Num() == 0)
		{
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("No assets selected."));
			return false;
		}

		
		bool bMaterialNameSet = false;
		
		for (const FAssetData& SelectedAssetData : SelectedDataToProccess)
		{
			UObject* SelectedAsset = SelectedAssetData.GetAsset(); //  直接获取这个资产对象的指针
			
			if (SelectedAsset == nullptr) continue;
			
			UTexture2D* SelectedTexture = Cast<UTexture2D>(SelectedAsset); //尝试将 UObject 转换为 UTexture2D 类型
			
			if ( !SelectedTexture )
			{
				DebugHeader::ShowMsgDialog(EAppMsgType::Ok,FString::Printf(TEXT("Selected asset %s is not a Texture2D."), *SelectedAssetData.AssetName.ToString()));
				return false;
			}
		
			OutSelectedTexturesArray.Add(SelectedTexture);
			
			if (OutSelectedTexturePackage.IsEmpty())
			{
				OutSelectedTexturePackage = SelectedAssetData.PackagePath.ToString();
				
			}
			
			if (!bCustomMaterialName && !bMaterialNameSet)
			{
				
				MaterialName = SelectedAsset->GetName();
				MaterialName.RemoveFromStart(TEXT("T_"));
				MaterialName.InsertAt(0,TEXT("M_"));
				
				bMaterialNameSet = true;
				
			}
			
			
			
		}
		
		
		return true;
		
	}

#pragma endregion