// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManger.h"

#include "ContentBrowserModule.h"
#include "ContentBrowserModule.h"

#define LOCTEXT_NAMESPACE "FSuperMangerModule"

void FSuperMangerModule::StartupModule()
{
	
	InitCBMenuExtention();
	
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module 此函数将在模块加载到内存后执行
}

void FSuperMangerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}


#pragma region ContentBrowserMenuExtention

	void FSuperMangerModule::InitCBMenuExtention()
	{
	
	FContentBrowserModule& ContentBrowserModule =
	FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser")); //LoadModuleChecked 这函数还有静态版本  所以不需要get就能用。。
	
	TArray<FContentBrowserMenuExtender_SelectedPaths>& ContentBrowserModuleExtenders = 
	ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	
	
	
	
	}

#pragma endregion



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSuperMangerModule, SuperManger)