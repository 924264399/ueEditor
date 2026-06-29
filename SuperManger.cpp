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
	ContentBrowserModule.GetAllPathViewContextMenuExtenders();  //返回的是委托数组 引用    FContentBrowserMenuExtender_SelectedPaths这玩意是类名
	
	
	FContentBrowserMenuExtender_SelectedPaths CustomCBMenuDelegate; //我们自定义委托（声明一个委托类型的实例对象）
	CustomCBMenuDelegate.BindRaw(this,&FSuperMangerModule::CustomCBMenuExtender); ////将委托绑定到入口函数
	
	
	ContentBrowserModuleExtenders.Add(CustomCBMenuDelegate);  //加到上面的委托数组里。
	
	/*
		 上面的创建委托 绑定到函数 添加到委托数组还可以使用  
		 ContentBrowserModuleExtenders.Add(FContentBrowserMenuExtender_SelectedPaths::CreateRow(this,&FSuperMangerModule::CustomCBMenuExtender))
		 这个方式 一口气完成
	*/
	
	}


//FExtender类 = 扩展规则的收纳容器，可以同时存放多个菜单栏 / 右键 / 工具栏扩展；（FExtender：总管容器，存一整批菜单栏 / 右键菜单 / 工具栏的扩展规则）
//FExtender类是 UE Slate 框架下专门用于 UI 菜单 / 工具栏扩展的普通 C++
//  const TArray<FString>& SelectedPaths  这个参数拿到用户当前选中的路径列表，这是右键菜单扩展的核心数据输入。
//下面这个是对接内容浏览器菜单扩展的唯一入口函数  执行3层绑定
TSharedRef<FExtender> FSuperMangerModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)    
{
	TSharedRef<FExtender> MenuExtender (new FExtender); // new FExtender先new业务对象    控制块由TSharedRef内部去new和管理
	
	if (SelectedPaths.Num()>0 )
	{
		
		MenuExtender->AddMenuExtension (
			FName("Delete"),
			EExtensionHook::After,
			TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateRaw(this,&FSuperMangerModule::AddCBMenuEntry) // 这玩意哪里冒出来的？？？
			
			); //第一层绑定 参数是插入的位置 关于插入的位置怎么看请看文档
				//第一个参数是插入位置 比如我们这里写的”Delete“
				//第二个参数是前还是后 这里选择after 就是插在delete后面
				//第三个参数是触发方式 如果期望是自定义的这里可以操作 如果不想就传入空的共享指针
				//第四个参数是第二个绑定 + 第三个绑定的函数，即菜单项的详细信息和业务逻辑函数 这里直接用一个函数来写AddCBenuEntry
														
		
		
		
	}
	
	
	
	return MenuExtender;
	
}

void FSuperMangerModule::AddCBMenuEntry( FMenuBuilder& MenuBuilder)
{
	//定义菜单项的具体内容
	
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(("Delete Unused Assets")),
		FText::FromString(TEXT("Safely delete all  Unused assets under folder")), //鼠标悬停在按钮上显示的
		FSlateIcon(),  //自定义图标  目前是写的一个空的
		FExecuteAction::CreateRaw(this,&FSuperMangerModule::OnDeleteUnsuedAssetsButtonClicked) //第三个绑定 业务函数
		
	);
	
}

void FSuperMangerModule::OnDeleteUnsuedAssetsButtonClicked()
{
	
}

#pragma endregion



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSuperMangerModule, SuperManger)