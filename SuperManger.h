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
	
	#pragma endregion 
	
	
};
