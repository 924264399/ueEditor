// Fill out your copyright notice in the Description page of Project Settings.


#include "quickassetactor/quickassetactor.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "IO/IoStatus.h"
//#include "Misc/MessageDialog.h"
#include "ObjectTools.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Materials/MaterialInstanceConstant.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"



void Uquickassetactor::TestFunction()
{
	Print(TEXT("Working"),FColor::Green);
	
	PrintLog(TEXT("Working"));
	
}

void Uquickassetactor::DuplicateAsset(int32 NumOfDuplicates)
{
	if (NumOfDuplicates <= 0)
	{
		ShowMsgDialog(EAppMsgType::Ok,TEXT("PLEASE ENTER A VALID NUMBER"));
		return;
		
	}
	
	TArray<FAssetData> SelectedAssetDate   = UEditorUtilityLibrary::GetSelectedAssetData(); 
	uint32 Counter = 0;
	
	for (const FAssetData& SelectedAssetData:SelectedAssetDate) // 这是专门遍历的数组的  把SelectedAssetDate的每一个元素赋值给 SelectedAssetData
	{
		for (int32 i=0;i<NumOfDuplicates;i++)  // NumOfDuplicates是用户输入的copy数量
		{
			const FString SourceAssetPath = SelectedAssetData.GetObjectPathString(); //获取SourcePath 这个资产位置
			const FString NewDuplicatedAssetName = SelectedAssetData.AssetName.ToString() + TEXT("_") + FString::FromInt(i-1); //为什么都要to string？因为Fname要先转String才能拼接
			const FString NewPathName = FPaths:: Combine(SelectedAssetData.PackageName.ToString(), NewDuplicatedAssetName);
			
			
			if (UEditorAssetLibrary::DuplicateAsset(SourceAssetPath,NewPathName))
			{
				UEditorAssetLibrary::SaveAsset(NewPathName,false); //把复制出来的新资产，从内存写入磁盘文件。 这个非常重要
				
				++Counter; 
				
			}
			
		}
		
		
	}
	
	
	if (Counter >0)
	{
		ShowNotifyInfo(TEXT("CopeOver"));
	}
	//PrintLog(TEXT("CopeOver")+FString::FromInt(Counter));	
}


void Uquickassetactor::AddPrefix()
{

	TArray<UObject*> SelectedObjects = UEditorUtilityLibrary::GetSelectedAssets();
	uint32 Counter = 0;


	for ( UObject* SelectedObject:SelectedObjects) //对这个TArray数组进行遍历
	{
		if(!SelectedObject) continue; //为什么会无效？？？ 

		
		FString* PrefixFound = PrefixMap.Find(SelectedObject->GetClass());  //通过这个资产实例获取他的UClass*  基于这个UClass* （key）  找到PrefixMap这个TMap中对应的FString的前缀 （value）


		if (!PrefixFound || PrefixFound->IsEmpty()) //比如你的Tmap里还没写到的一些类 那么检索出来的PrefixFound就是空的
		{
			PrintLog(TEXT("Failed to find prefix for class") + SelectedObject->GetClass()->GetName());
			continue;
		}

		FString OldName = SelectedObject->GetName();

		if (OldName.StartsWith(*PrefixFound))  //检测这个FString的前缀  如果和PrefixFound一致那就是已经添加了正确的前缀
		{
			Print(OldName + TEXT(" already has prefix addad"), FColor::Red);
			continue;
		}
		
		
		if (SelectedObject->IsA<UMaterialInstanceConstant>())
		{
			OldName.RemoveFromStart(TEXT("M_"));
			OldName.RemoveFromEnd(TEXT("_Inst"));
			
		}
		
		

		const FString NewName =  *PrefixFound + OldName;


		UEditorUtilityLibrary::RenameAsset(SelectedObject, NewName);

		++Counter;

	}

	if (Counter >0)	
	{
		
	ShowNotifyInfo(TEXT("RenameOver"));
		
	}

	

}



//这个逻辑比较简单，重点是FindPackageReferencersForAsset 这个方法可以输出资产的所有引用到一个TArray
//那么如果这个TArray为0 那就是这个资产没有被任何引用喽~~  就可以进到UnusedAssets的TArray 最好被DeleteAssets自由函数删除

void Uquickassetactor::RemoveUnusedAssets()
{
	
	TArray<FAssetData> SelectedAssetDates   = UEditorUtilityLibrary::GetSelectedAssetData(); 
	
	TArray<FAssetData> UnusedAssets ;
	
	// 这里还可以调用FixUpRedirectors函数
	
	for (const FAssetData& SelectedAssetData:SelectedAssetDates)
	{
		
		TArray<FString> AssetRefrencers = UEditorAssetLibrary:: FindPackageReferencersForAsset(SelectedAssetData.GetObjectPathString()); //找包级别的引用关系 它返回的是所有引用者的完整包路径字符串
		
		if (AssetRefrencers.Num() == 0)
		{
			UnusedAssets.Add(SelectedAssetData);
			
		}
		
	}
	
	if (UnusedAssets.Num() == 0)
	{
		
		ShowMsgDialog(EAppMsgType::Ok,TEXT("No unused assets found"),false);
		return;
		
	}
	
	const int32 NumOfUnusedDeleted = ObjectTools::DeleteAssets(UnusedAssets); //ObjectTools是一个name space  这是namespace + 自由函数  这样比设计成工具类更快
	
	if (NumOfUnusedDeleted == 0) return;
	
	
	ShowNotifyInfo(TEXT("Success deleted"));
	
	
	
}



//更新重定向引用
void Uquickassetactor::FixUpRedirectors()
{
	//// 存放待修复的资源重定向器的TArray
	TArray<UObjectRedirector*> RedirectorsToFixArray; //UObjectRedirectors个类  就是UE里表示“资源重定向器”的类。 
	
	
	//<FAssetRegistryModule> 是 C++ 模板参数   请按 FAssetRegistryModule 这个类型来加载/返回 AssetRegistry 模块。
	
	// 	模块管理器，加载名叫 AssetRegistry 的模块。返回他的模块类（入口类）FAssetRegistryModule给我
	FAssetRegistryModule& AssetRegistryModule = 
	FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry")); //FModuleManager是模块管理器  Load加载资产注册表模板  后面可以用它查找项目里的资源、路径、重定向器
																							//他管理诸如 core Engine 这些模块 具体的行为（加载 卸载  查找）
																							//主要是因为 AssetRegistry 不是你当前类自带的功能，而是 UE 的一个独立模块
																							//所以你必须加载进来先
	
																							//而AssetRegistry是 UE 编辑器维护的一张“资产索引表
																							//他记录项目里有哪些资产的各种信息 名称 路径 依赖 引用 类型
																							//为什么选中资产不需要 AssetRegistry？而找重定向引用需要？  因为后者范围很大啊~ 你需要遍历资产注册表，帮我找出所有类型是 UObjectRedirector 的资产。（你不知在哪 有几个）
	
	FARFilter Filter;  // 资产过滤器  是帅选资产的调节  是通用的吗？
	Filter.bRecursivePaths = true;  //表示递归搜索路径。
	Filter.PackagePaths.Emplace("/Game"); //把 /Game 这个路径加入筛选条件。  Game 代表项目的 Content 目录 
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
