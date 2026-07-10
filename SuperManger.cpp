// Copyright Epic Games, Inc. All Rights Reserved.

#include "SuperManger.h"

#include "ContentBrowserModule.h"
#include "ContentBrowserModule.h"
#include "DebugHeader.h"
#include "EditorAssetLibrary.h"
#include "ObjectTools.h"
#include "AssetToolsModule.h"

#include "AssetRegistry/AssetRegistryModule.h"

#define LOCTEXT_NAMESPACE "FSuperMangerModule"

void FSuperMangerModule::StartupModule()
{
	
	InitCBMenuExtention();
	
	RegisterAdvanceDeletionTab();
	
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
TSharedRef<FExtender> FSuperMangerModule::CustomCBMenuExtender(const TArray<FString>& SelectedPaths)    //这个TArray是自动传参的 不需要自己去定义 （在用户右键触发后这个TArray自动生成）
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
														
		
		FolderPathsSelectedPaths = SelectedPaths; //如果用户选择了文件夹 我们就把这个文件夹的数组转移到我们的变量  给后面的具体逻辑用
		
	}
	
	
	
	return MenuExtender;
	
}

void FSuperMangerModule::AddCBMenuEntry( FMenuBuilder& MenuBuilder)
{
	//定义菜单项的具体内容
	
	//这个我们自己的数组是MenuBuilder去维护的
	
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(("Delete Unused Assets")),
		FText::FromString(TEXT("Safely delete all  Unused assets under folder")), //鼠标悬停在按钮上显示的
		FSlateIcon(),  //自定义图标  目前是写的一个空的
		FExecuteAction::CreateRaw(this,&FSuperMangerModule::OnDeleteUnsuedAssetsButtonClicked) //第三个绑定 业务函数
		
	);
	
	
	MenuBuilder.AddMenuEntry
	(
		FText::FromString(("Delete Empty Folders")),
		FText::FromString(TEXT("Safely delete all  empty folder")), //鼠标悬停在按钮上显示的
		FSlateIcon(),  //自定义图标  目前是写的一个空的
		FExecuteAction::CreateRaw(this,&FSuperMangerModule::OnDeleteEmptyFolders) //第三个绑定 业务函数
			
	);
	
	
		MenuBuilder.AddMenuEntry
	(
		FText::FromString(("Advance Delete")),
		FText::FromString(TEXT("List assets by specific condition in a tab for deleting")), //鼠标悬停在按钮上显示的
		FSlateIcon(),  //自定义图标  目前是写的一个空的
		FExecuteAction::CreateRaw(this,&FSuperMangerModule::OnAdvanceDeleteButtonClicked) //第三个绑定 业务函数
				
	);
	
}


void FSuperMangerModule::OnDeleteEmptyFolders()
{
	FixUpRedirectors();
	
	TArray<FString> FolferPathsArray = UEditorAssetLibrary::ListAssets(FolderPathsSelectedPaths[0],true,true); //获取文件夹下的所有资产的路径  返回到一个TArray数组
	
	FString EmptyFolderPathsNames;
	TArray<FString> EmptyFolderPathsArray; //空文件夹的列表
	
	for (const FString& FolderPath : FolferPathsArray)
	{
		if (FolderPath.Contains(TEXT("Developers")) || FolderPath.Contains(TEXT("Collections")) )  //不允许删除额的文件夹
		{
		
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("The folder is cant be delete!!"));
			continue;
		}
		
		
		if (!UEditorAssetLibrary::DoesDirectoryExist(FolderPath)) continue;  //如果目录不存在 就说明是资源 而不是目录？  这里不理解
		
		if (!UEditorAssetLibrary::DoesDirectoryHaveAssets(FolderPath)) //如果目录下没有资产 就说明是空目录
		{
			EmptyFolderPathsNames.Append(FolderPath);
			EmptyFolderPathsNames.Append( TEXT("\n"));
			
			EmptyFolderPathsArray.Add(FolderPath); //数组添加
			
		}
		
	};
	
	if (EmptyFolderPathsArray.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("No empty folders found!"));
		return;
		
	}
	
	EAppReturnType::Type ConfimResult = DebugHeader::ShowMsgDialog(EAppMsgType::OkCancel, TEXT("The following empty folders will be deleted:\n") + EmptyFolderPathsNames,false); // 询问用户是否删除
	
	
	if (ConfimResult == EAppReturnType::Ok)
	{
		for (const FString& EmptyFolderPath : EmptyFolderPathsArray)
		{
			UEditorAssetLibrary::DeleteDirectory(EmptyFolderPath); //删除空文件夹
		}
		
		DebugHeader::ShowNotifyInfo(TEXT("Empty folders deleted successfully!"));
		
	}
	else
	{
		DebugHeader::ShowNotifyInfo(TEXT("Empty folder deletion canceled."));
	}
	
	
}







void FSuperMangerModule::OnDeleteUnsuedAssetsButtonClicked()
{
	
	
	if (FolderPathsSelectedPaths.Num() > 1)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("Please select only one folder!"));
		
		return;
	}
	
	TArray<FString> AssetsPathNames = UEditorAssetLibrary::ListAssets(FolderPathsSelectedPaths[0]); //获取文件夹下的所有资产的路径  返回到一个TArray数组
	
	if (AssetsPathNames.Num() == 0) 
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("The folder is empty!"));
		
		return;
	}
	
	
	//在这里先修复重定向器 我可以先获取资产注册表 然后筛选。。。
	FixUpRedirectors();
	
	TArray<FAssetData> UNUsedAssetsDataAtrray;
	
	for (const FString& AssetPathName : AssetsPathNames)	
	{
		
		//过滤敏感文件夹 就是放编译的这些
		if (AssetPathName.Contains(TEXT("Developers")) || AssetPathName.Contains(TEXT("Collections")) )
		{
		
			DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("The folder is cant be delete!!"));
			continue;
		}
		
		if ( !UEditorAssetLibrary::DoesAssetExist(AssetPathName) ) continue;  //如果资源不存在 就跳过
		
		
		TArray<FString> AssetPackageRefrencers = UEditorAssetLibrary:: FindPackageReferencersForAsset(AssetPathName); //获取引用这个资源的所有资源的路径
		
		
		if (AssetPackageRefrencers.Num() == 0)
		{
			
			const FAssetData UnusedAssetData = UEditorAssetLibrary::FindAssetData(AssetPathName); //获取这个资源的FAssetData
			UNUsedAssetsDataAtrray.Add(UnusedAssetData); //添加到数组里
			
		}
		
		
		
	}
		
	
	if (UNUsedAssetsDataAtrray.Num() > 0)
	{
		ObjectTools::DeleteAssets(UNUsedAssetsDataAtrray); //删除资源	
		
	}
	else
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("No unused assets found!"));
		
	}
	
}

void FSuperMangerModule::OnAdvanceDeleteButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(FName("AdvanceDeletionTab")); //打开我们自定义的标签页
	
	
}


void FSuperMangerModule::FixUpRedirectors()
{
	
	//// 存放待修复的资源重定向器的TArray
	TArray<UObjectRedirector*> RedirectorsToFixArray; //UObjectRedirectors个类  就是UE里表示“资源重定向器”的类。
	
	
	FAssetRegistryModule& AssetRegistryModule = 
	FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")); //加载资产注册表模板
	
	
	FARFilter Filter;  // 资产过滤器  是帅选资产的调节  是通用的吗？
	Filter.bRecursivePaths = true;  //表示递归搜索路径。
	Filter.PackagePaths.Emplace("/Game"); //把 /Game 这个路径加入筛选条件。  Game 代表项目的 Content 目录   这里依旧是全部文件夹的 比较保险
	Filter.ClassNames.Emplace("ObjectRedirector");  //表示只查类型是 ObjectRedirector 的资产。  就是只找重定向器，不找贴图、材质、蓝图、StaticMesh 等等
	
	TArray<FAssetData>  OutRedirectors; //创建一个FAssetData数组  用来接受查到的资产
	AssetRegistryModule.Get().GetAssets(Filter, OutRedirectors);  //让资产注册表按照 Filter 的条件查资产。
	
	for(const FAssetData& RedirectorData:OutRedirectors)  //遍历每一个重定向器资产信息
	{
		if (UObjectRedirector* RedirectorToFix = Cast<UObjectRedirector>(RedirectorData.GetAsset()))//用GetAsset把他变成UObject*    然后把UObject* 转成 UObjectRedirector*（用这个Cast转换）
			
			RedirectorsToFixArray.Add(RedirectorToFix); //加到上面数组里面了
	
	}
	
	FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools")); //和上面一样 只不过这次拿的是AssetTools模块
	
	AssetToolsModule.Get().FixupReferencers(RedirectorsToFixArray);
	
	
	
}


#pragma endregion





#pragma region CustomEditorTab

    //通过这个函数可以创建独立窗口  标签页
	//此函数必须在StartupModule注册
	void FSuperMangerModule::RegisterAdvanceDeletionTab()
	{
		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(FName("AdvanceDeletionTab"), 
			FOnSpawnTab::CreateRaw(this, &FSuperMangerModule::OnSpawnAdvanceDeletionTab))
			.SetDisplayName(FText::FromString(TEXT("Advance Deletion")));
														//这就是创建slate的独立窗口的最重要的api  其中的 tabID 你知道的
														//FGlobalTabmanager::Get() 返回的是一个智能指针
														//RegisterNomadTabSpawner （ tabid ， 创建一个委托）
	
	}

	TSharedRef<SDockTab> FSuperMangerModule::OnSpawnAdvanceDeletionTab(const FSpawnTabArgs& TabArgs )
	{
	
		return  SNew(SDockTab).TabRole(NomadTab);
	
		//return TSharedRef<SDockTab>(); //TSharedRef必须指向有效对象 还需要一个sdog选项卡
	
	}

#pragma endregion
	



#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FSuperMangerModule, SuperManger)