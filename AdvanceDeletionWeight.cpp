// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWights/AdvanceDeletionWeight.h"

#include "DebugHeader.h"
#include "SlateBasics.h"
#include "Components/Widget.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "DebugHeader.h"
#include "SuperManger.h"

#define ListALL TEXT("List All Available Assets")
#define ListUnused TEXT("List Unused Assets")
#define ListSameName TEXT("List Assets With Same Name")

void ASdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;  //是 SWidget 基类里的一个布尔成员变量，表示这个控件能不能获得键盘焦点（ 即能接受键盘输入  激活些什么）
	
	
	
	StoredAssetsData = InArgs._AssetsDataToStore;  //把参数传入到成员变量里  
	DisplayAssetsData = StoredAssetsData;  //一开始 这两个数组是完全一样的
	
	CheckBoxesArray.Empty(); //情况复选框数组
	AssetsDataToDeleteArray.Empty(); 
	ComBoxSourceItems.Empty();  //清空下拉菜单的数组
	
	ComBoxSourceItems.Add(MakeShared<FString>(ListALL) );
	ComBoxSourceItems.Add(MakeShared<FString>(ListUnused) );
	ComBoxSourceItems.Add(MakeShared<FString>(ListSameName) );//列出同名资产
	
	FSlateFontInfo TitleTextFontInfo = GetEmboseedTestFont();  //设置字体样式
	
	// ChildSlot  // 默认槽位
	// [
	// 	SNew(STextBlock)
	// 	.Text(FText::FromString(InArgs._TestString))  //把参数显示在文本框上   InArgs._TestString 就之前 SLATE_ARGUMENT(FString, TestString) 声明的参数
	// 												 //		InArgs是结构体   _TestString是里面的参数
	// 	
	// ];
	
	ChildSlot
	[
		SNew(SVerticalBox)  //垂直盒子控件 里面可以放多个槽位  这样我们才能放入更多的东西比如多个文本

		//标题文本的第一个垂直槽
		+ SVerticalBox::Slot()  //用这个来添加槽位
		.AutoHeight()  // 设置槽位属性 这里是自动高度
		[
			SNew(STextBlock) //在槽位里创建一个文本块
			.Text(FText::FromString(TEXT("Advance Deletion")))  //设置文本内容
			.Font(TitleTextFontInfo)  //设置字体样式
			.Justification(ETextJustify::Center)//居中对其
			.ColorAndOpacity(FColor::Blue) //设置颜色
		]

		//第二个槽位是下拉菜单  以指定列表条件
		+ SVerticalBox::Slot()
		.AutoHeight()
		[

			SNew( SHorizontalBox)  //水平盒子控件  从做到右放置多个槽位


			+SHorizontalBox::Slot()
			.AutoWidth()
			[

			//创建下拉菜单控件	
				ConstructComboBox()
				
			]
			
			
		]
		
		//第三个槽位是资产列表
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)//垂直对齐-让槽位里的控件在垂直方向填满这个 Slot 给它的高度
		[

			SNew( SScrollBox)  // 滚动框控件


			+ SScrollBox::Slot() //给滚动框增加一个槽位
			[
				ConstructAssetListView()
			 	
			]
			
		]
		
		//第四个槽来放三个按钮
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
		
			SNew( SHorizontalBox) //水平盒子

			//删除所有按钮
			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructDeleteALLButton() //吹昂加删除所有的按钮
			]
			
			
			//全选所有按钮
			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructSelectedAllButton() //创建全选所有的按钮
			]

			//删除所选assets 的按钮
			+SHorizontalBox::Slot()
			.FillWidth(10.f)
			.Padding(5.f)
			[
				ConstructDeleteSelectedAllButton() //创建删除所选assets 的按钮
			]
			
			
		]
	
	];
	
	
}


//创建slistview 的函数
TSharedRef<SListView<TSharedPtr<FAssetData>>> ASdvanceDeletionTab::ConstructAssetListView()
{
	ConstructedAssetListView = 
	SNew(SListView<TSharedPtr<FAssetData>>)  //列表控件  里面是我们传入的资产数据类型
	.ItemHeight(24)  //设置每一行的高度
	.ListItemsSource(&DisplayAssetsData)  //设置listview的  列表数据源  
	.OnGenerateRow(this,&ASdvanceDeletionTab::OnGenerateRowForList) 
	.OnMouseButtonClick(this,&ASdvanceDeletionTab::OnRowWidgetMoustButtonClicked);  //鼠标点击事件	
	
	
	return ConstructedAssetListView.ToSharedRef();
	
}



void ASdvanceDeletionTab::RefreshAssetListView()
{
	CheckBoxesArray.Empty();
	AssetsDataToDeleteArray.Empty();
	
	if (ConstructedAssetListView.IsValid())
	{
		ConstructedAssetListView->RebuildList();
		
	}
}



#pragma region 	ComboBoxForListingCondition
	
TSharedRef<SComboBox<TSharedPtr<FString>>> ASdvanceDeletionTab::ConstructComboBox()
{
	
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructedComboBox = 
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource( &ComBoxSourceItems )   //选项源 决定有哪些选项   一般有好几个 所以放在数组里
		.OnGenerateWidget(this,&ASdvanceDeletionTab::OnGenerateComboContent)  //决定下拉里面是什么控件  这是遍历的哈
		.OnSelectionChanged(this,&ASdvanceDeletionTab::OnComboSelectionChanged)
		[

			//这里则是下拉框关闭时，SComboBox 本身显示什么内容。（框体上显示什么）
			SAssignNew(ComboDiplayTextBlock,STextBlock)
			.Text(FText::FromString(TEXT("List Assets Option")))
			
		];
		
	
	return ConstructedComboBox;
}


TSharedRef<SWidget> ASdvanceDeletionTab::OnGenerateComboContent(TSharedPtr<FString> SourceItem)
{
	TSharedRef<STextBlock> ContructedComboBox = SNew(STextBlock)
		.Text(FText::FromString(*SourceItem.Get()));
	
	return ContructedComboBox;
	
}

void ASdvanceDeletionTab::OnComboSelectionChanged(TSharedPtr<FString> SelectedOption, ESelectInfo::Type InSelectInfo)//SelectedOption 就是用户选择的
{
	//DebugHeader::Print(*SelectedOption.Get(),FColor::Green);
	
	ComboDiplayTextBlock->SetText(FText::FromString(*SelectedOption.Get()));
	
	//我们要用模块的函数来筛选fassetsdata 改变DisplayAssetsData数组
	
	FSuperMangerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperMangerModule>( TEXT("SuperManger") );
	
	if ( *SelectedOption.Get() == ListALL)
	{
		//列出所有的
		DisplayAssetsData = StoredAssetsData;
		RefreshAssetListView();
		
	}
	
	if ( *SelectedOption.Get() == ListUnused)
	{
		//进行筛选 未使用的
		
		SuperManagerModule.ListUnusedAssetsForAssetList(StoredAssetsData,DisplayAssetsData);  //筛选然和填充DisplayAssetsData
		RefreshAssetListView();
		
	}
	
	if ( *SelectedOption.Get() == ListSameName)
	{
		//列出所有同名资源
		SuperManagerModule.ListSameNameAssetsForAssetList(StoredAssetsData,DisplayAssetsData);  //筛选然和填充DisplayAssetsData
		RefreshAssetListView();
		
	}
	
	
}


#pragma endregion  	
	




#pragma region RowWidgetForAssetListView 	



// 这里的AssetDataToDisplay 是怎么和StoredAssetsData 关联的？  答案是在.ListItemsSource(&StoredAssetsData)这一步  是自动关联的
//如果 StoredAssetsData 里有 10 个资产，OnGenerateRowForList 大概会被调用 10 次，每次的 AssetDataToDisplay 都是不一样的数组元素。
TSharedRef<ITableRow> ASdvanceDeletionTab::OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	if (!AssetDataToDisplay.IsValid()) return SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable);  //如果传入的资产数据无效 就返回一个空的行控件
	
	const FString DisplayAssetClassName = AssetDataToDisplay->AssetClassPath.GetAssetName().ToString();
	const FString DisplayAssetName = AssetDataToDisplay->AssetName.ToString(); //资源类名
	
	FSlateFontInfo AssetClassNameFont = GetEmboseedTestFont();  //设置类名字体样式
	AssetClassNameFont.Size = 10;  //设置字体大小
	
	FSlateFontInfo AssetNameFont = GetEmboseedTestFont();  //设置资产名称字体样式
	AssetNameFont.Size = 10;  //设置字体大小

	TSharedRef<STableRow<TSharedPtr<FAssetData>>> ListViewRowWidget =  
	SNew(STableRow<TSharedPtr<FAssetData>>, OwnerTable)
		.Padding(FMargin(5.f))   //内边距  相对和每行的边界有0.5的距离
		[

			SNew(SHorizontalBox)

			//第一个槽位是给复选框 用复选框判断 该资源是否被选中
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Left)// 这个槽位左对其  保证复选框靠左
			.VAlign(VAlign_Center)// 这个槽位垂直居中  保证复选框在中间
			.FillWidth(.05f)//这个槽占的空间
			[

				ConstructCheckBox(AssetDataToDisplay)  //  用来生成复选框的逻辑 这样更具可读性
				
			]
			

			//第二个槽位用于显示资源名称 资源类名
			+SHorizontalBox::Slot()
			.HAlign(HAlign_Center)// 这个槽位水平居中  
			.VAlign(VAlign_Fill)// 这个槽位垂直填充  保证资源名称在中间
			.FillWidth(.5f)
			[

				ConstructTextForRowWidget(DisplayAssetClassName,AssetClassNameFont)  //  用来生成文本控件的逻辑

				
			]
			
			//第三个位置显示资源名称

			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center) 
			.VAlign(VAlign_Fill)
			[
	
				ConstructTextForRowWidget(DisplayAssetName,AssetNameFont)  //  用来生成文本控件的逻辑
				
			]
			
			

			//第四个位置放一个按钮（点击这个按钮直接删除资源）
			+ SHorizontalBox::Slot()
			.HAlign(HAlign_Center) //右靠边
			.VAlign(VAlign_Fill)
			[
				
				ConstructButtonForRowWidget(AssetDataToDisplay)  //  用来生成按钮控件的逻辑

				
				
			]
			
			
		];
	
	
	return ListViewRowWidget;
	
	
	//STableRow<TSharedPtr<FAssetData>> 是一个具体的 Slate 行控件    它可以被当成 ITableRow 使用
	//ITableRow = “一行控件应该具备的接口/规矩”
	//STableRow = “UE 已经写好的标准行控件实现”
		
	
}

void ASdvanceDeletionTab::OnRowWidgetMoustButtonClicked(TSharedPtr<FAssetData> AssetDataToDisplay) //行控件的鼠标点击事件回调
{
	
	FSuperMangerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperMangerModule>( TEXT("SuperManger") );
	
	SuperManagerModule.SyncCBToClickedAssetForAssetList(AssetDataToDisplay->ObjectPath.ToString());
	
	
}

TSharedRef<SCheckBox> ASdvanceDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData> AssetDataToDisplay)
{
	
	
	TSharedRef<SCheckBox> ConstructedCheckBox  =
	 SNew(SCheckBox)
	.Type(ESlateCheckBoxType::CheckBox) //  这里CheckBox的类型是复选框  还有切换按钮的类型
	.OnCheckStateChanged(this, &ASdvanceDeletionTab::OnCheckBoxStateChanges,AssetDataToDisplay) //依然绑定回调函数
	.Visibility(EVisibility::Visible); //可见性是可见
	
	CheckBoxesArray.Add(ConstructedCheckBox); //我们把复选框加入数组  注意行已经是循环调用 所以我们这里是不需要用循环去加的
	
	return ConstructedCheckBox;
	
	
}

void ASdvanceDeletionTab::OnCheckBoxStateChanges(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	
	switch (NewState)  //NewState 是枚举
	{
		case ECheckBoxState::Checked: //选中
		
		AssetsDataToDeleteArray.AddUnique(AssetData); //用AddUnique 保证数组不添加重复项
		
			break;
		
		case ECheckBoxState::Unchecked: //取消选中
		
		if (AssetsDataToDeleteArray.Contains(AssetData))
		{
			AssetsDataToDeleteArray.Remove(AssetData);
			
		}		
		
			break;
		
		
		case ECheckBoxState::Undetermined: //不确定
			break;  
		
		default:
			break;
		
		
	}
	
	
	
}


//文本框控件 生成函数
TSharedRef<STextBlock> ASdvanceDeletionTab::ConstructTextForRowWidget(const FString& TextContent,
	const FSlateFontInfo& FontInfo)
{
	TSharedRef<STextBlock> ConstructedTextBlock =
		SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(FontInfo)
		.ColorAndOpacity(FColor::White);
	
	return ConstructedTextBlock;
}




//按钮
TSharedRef<SButton> ASdvanceDeletionTab::ConstructButtonForRowWidget(const TSharedPtr<FAssetData> AssetDataToDisplay)
{
	
	TSharedRef<SButton> ConstructedButton =
	SNew(SButton)
	.Text(FText::FromString(TEXT("Delete"))) // 按钮的文本
	.OnClicked(this, &ASdvanceDeletionTab::OnDeleteButttonClicked, AssetDataToDisplay); // 按钮点击事件绑定

	
	return ConstructedButton;
}


//回调函数 用于按钮的
FReply ASdvanceDeletionTab::OnDeleteButttonClicked(TSharedPtr<FAssetData> AssetDataToDisplayData)
{
	
	
	FSuperMangerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperMangerModule>( TEXT("SuperManger") );
	
	const bool bAssetDeleted =  SuperManagerModule.DeleteSingleAssetsForAssetList(*AssetDataToDisplayData.Get()); 
	
	//刷新列表
	if (bAssetDeleted)
	{
		
		
		//先把这个东西从数组里移除
		if (StoredAssetsData.Contains(AssetDataToDisplayData))
		{
			
			StoredAssetsData.Remove(AssetDataToDisplayData);
			
		}
		
		if (DisplayAssetsData.Contains(AssetDataToDisplayData))
		{
			
			DisplayAssetsData.Remove(AssetDataToDisplayData);
			
		}
		
		
		
		//刷新列表
		RefreshAssetListView();
		
		
		
		
	}
	
	
	
	return FReply::Handled();
	
}



#pragma endregion



#pragma region TabButtons

TSharedRef<SButton> ASdvanceDeletionTab::ConstructDeleteALLButton()
{
	TSharedRef<SButton> DeleteAllButton = 
		SNew(SButton)
		.ContentPadding(FMargin(5.f))
		.OnClicked(this, &ASdvanceDeletionTab::OnDeleteAllButtonClicked);
	
	DeleteAllButton->SetContent(ConstructTextForTabButtons(TEXT("DeleteAllButton")));
	
	return DeleteAllButton;
	
}

TSharedRef<SButton> ASdvanceDeletionTab::ConstructSelectedAllButton()
{
	
	TSharedRef<SButton> SelectedAllButton = 
		SNew(SButton)
		.ContentPadding(FMargin(5.f))
		.OnClicked(this, &ASdvanceDeletionTab::OnSelectedAllButtonClicked);
	
	SelectedAllButton->SetContent(ConstructTextForTabButtons(TEXT("SelectedAllButton")));
	
	return SelectedAllButton;
	
}

TSharedRef<SButton> ASdvanceDeletionTab::ConstructDeleteSelectedAllButton()
{
	TSharedRef<SButton> DeleteSelectedAllButton = 
		SNew(SButton)
		.ContentPadding(FMargin(5.f))
		.OnClicked(this, &ASdvanceDeletionTab::OnDeleteSelectedAllButtonClicked);
	
	DeleteSelectedAllButton->SetContent(ConstructTextForTabButtons(TEXT("DeleteSelectedAllButton")));
	
	return DeleteSelectedAllButton;
	
}



FReply ASdvanceDeletionTab::OnDeleteAllButtonClicked()
{
	if (AssetsDataToDeleteArray.Num() == 0)
	{
		DebugHeader::ShowMsgDialog(EAppMsgType::Ok,TEXT("NO ASSET CURRENTLY SELECTED"));
		return  FReply::Unhandled();
	}

	
	TArray<FAssetData> AssetDataToDelete;
	
	for (const TSharedPtr<FAssetData>& Data : AssetsDataToDeleteArray)
	{
		
		AssetDataToDelete.Add(*Data.Get());  // 这里把TSharedPtr解引用 获得了真正的资产Fassetdata 然后加到一个数组里  这个数组就可以去调用supermanager模块里的删除的方法了
		
		
	}
	
	FSuperMangerModule& SuperManagerModule = FModuleManager::LoadModuleChecked<FSuperMangerModule>( TEXT("SuperManger") );
	
	const bool bAssetDeleted =  SuperManagerModule.DeleteMultipleAssetsForAssetList(AssetDataToDelete);
	
	//记得删完刷新列表
	if (bAssetDeleted == true)
	{
		
		for (const TSharedPtr<FAssetData>& DeletedData : AssetsDataToDeleteArray)
		{
			if (StoredAssetsData.Contains(DeletedData))
			{
				
				StoredAssetsData.Remove(DeletedData);  //这是最初的那个数组
				
			}
			
			
			
			if (DisplayAssetsData.Contains(DeletedData))
			{
				
				
				DisplayAssetsData.Remove(DeletedData);
			}
			
			
				
		}	

		
		//刷新列表
		RefreshAssetListView();
		
		
	}
	
	
	return FReply::Unhandled();
	
}


FReply ASdvanceDeletionTab::OnSelectedAllButtonClicked() //全选复选框
{
	
	if (CheckBoxesArray.Num() == 0)
	{
		return FReply::Unhandled();
	}
	
	for (const TSharedRef<SCheckBox>& CheckBox : CheckBoxesArray)
	{
		if (!CheckBox->IsChecked()) //检查复选框的选中状态
		{
			CheckBox->ToggleCheckedState(); //这个方法是切换复选框的状态  选中的就变成没选中  没选中的就选中
		}
	}
	
	return FReply::Unhandled();
}






FReply ASdvanceDeletionTab::OnDeleteSelectedAllButtonClicked()  //取消全选复选框
{
	
	if (CheckBoxesArray.Num() == 0)
	{
		return FReply::Unhandled();
	}
	
	for (const TSharedRef<SCheckBox>& CheckBox : CheckBoxesArray)
	{
		if (CheckBox->IsChecked()) //检查复选框的选中状态
		{
			CheckBox->ToggleCheckedState(); //这个方法是切换复选框的状态  选中的就变成没选中  没选中的就选中
		}
	}
	
	return FReply::Unhandled();
	
}



//用来设置文本快
TSharedRef<STextBlock> ASdvanceDeletionTab::ConstructTextForTabButtons(const FString& TextContent)
{
	FSlateFontInfo ButtonTextFont = GetEmboseedTestFont(); //从 GetEmboseedTestFont() 获得一份基础字体配置   ButtonTextFont里面包含了一堆字体信息 大小 形状 边距 字形  这样就不用在下面慢慢设置了
	ButtonTextFont.Size = 15;
	
	TSharedRef<STextBlock> ConstructedTextBlock = SNew(STextBlock)
		.Text(FText::FromString(TextContent))
		.Font(ButtonTextFont)
		.ColorAndOpacity(FColor::White)
		.Justification(ETextJustify::Center);//对其方式居中
	
	return ConstructedTextBlock;
}

#pragma endregion 

