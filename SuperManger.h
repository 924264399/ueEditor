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
	
	void AddCBMenuEntry(class FMenuBuilder& MenuBuilder );
	
	void OnDeleteUnsuedAssetsButtonClicked(); //业务函数  删除未使用的资产
	
	
	
	
	#pragma endregion 
	
	
};
