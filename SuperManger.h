// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

class FSuperMangerModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	
	
private:
	

	
	#pragma region ContentBrowserMenuExtention
	
		void InitCBMenuExtention();
	
	
	TSharedRef<FExtender> CustomCBMenuExtender( const TArray<FString>& SelectedPaths); //返回值是一个共享引用 智能指针（不可为空）
	
	TArray<FString> FolderPathsSelectedPaths;
	
	void AddCBMenuEntry(class FMenuBuilder& MenuBuilder );
	
	void OnDeleteUnsuedAssetsButtonClicked(); //业务函数  删除未使用的资产
	
	void OnAdvanceDeleteButtonClicked(); 
	
	void OnDeleteEmptyFolders();
	
	void FixUpRedirectors();	
	
	
	#pragma endregion 
	
	
	
	
	#pragma region CustomEditorTab
	
	
		void RegisterAdvanceDeletionTab();
	
		TSharedRef<SDockTab> OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& TabArgs); //这的函数的 参数和返回类型都是基于FOnSpawnTab::CreateRaw 这个委托所需要的
	
		TArray<TSharedPtr<FAssetData>> GetAllAssetsDataUnderSelectedFolder();
	
	
	
	#pragma endregion
	
	
	
public:	
	
	#pragma region ProccessDataForAdvanceDeletionTab
	
		bool DeleteSingleAssetsForAssetList(const FAssetData& AssetDataToDelete);
		bool DeleteMultipleAssetsForAssetList(const TArray<FAssetData>& AssetsToDelete);
	
		void ListUnusedAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsDataToFilter,TArray<TSharedPtr<FAssetData>>& OutUnusedAsstesData);
		void ListSameNameAssetsForAssetList(const TArray<TSharedPtr<FAssetData>>& AssetsDataToFilter,TArray<TSharedPtr<FAssetData>>& OutSameNameAsstesData);
	
	#pragma endregion
	
	
	
	
};
