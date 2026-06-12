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
