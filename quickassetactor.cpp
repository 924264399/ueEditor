// Fill out your copyright notice in the Description page of Project Settings.


#include "quickassetactor/quickassetactor.h"
#include "DebugHeader.h"
#include "EditorUtilityLibrary.h"
#include "EditorAssetLibrary.h"
#include "IO/IoStatus.h"
//#include "Misc/MessageDialog.h"


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

	TArray<UObject*> SelectedObjects = UEditorAssetLibrary::GetSelectedAssets();
	uint32 Counter = 0;


	for (const UObject* SelectedObject:SelectedObjects) //对这个TArray数组进行遍历
	{
		if(!SelectedObject) continue; //为什么会无效？？？ 

		
		FString* PrefixFound = PrefixMap.Find(SelectedObject->GetClass());  //通过这个资产实例获取他的UClass*  基于这个UClass* （key）  找到PrefixMap这个TMap中对应的FString的前缀 （value）


		if (!PrefixFound || PrefixFound->IsEmpty()) //比如你的Tmap里还没写到的一些类 那么检索出来的PrefixFound就是空的
		{
			PrintLog(TEXT("Failed to find prefix for class") + SelectedObject->GetClass()->GetName(), FColor::Red);
			continue;
		}

		FString OldName = SelectedObject->GetName();

		if (OldName.StartsWith(*PrefixFound))  //检测这个FString的前缀  如果和PrefixFound一致那就是已经添加了正确的前缀
		{
			Print(OldName + TEXT(" already has prefix addad"), FColor::Red);
		}

		const FString NewName =  *PrefixFound + OldName;


		UEditorUtilityLibrary::RenameAsset(SelectedObject, NewName);

		++Counter;

	}

	if (Counter >0)	
	{


	ShowNotifyInfo(TEXT("RenameOver"));}

	}

}