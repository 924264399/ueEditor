 // Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWights/AdvanceDeletionWeight.h"

#include "DebugHeader.h"
#include "SlateBasics.h"
#include "Components/Widget.h"
#include "DynamicMesh/DynamicMesh3.h"
#include "DebugHeader.h"
#include "SuperManger.h"


void ASdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;  //是 SWidget 基类里的一个布尔成员变量，表示这个控件能不能获得键盘焦点（ 即能接受键盘输入  激活些什么）
	
	
	
	StoredAssetsData = InArgs._AssetsDataToStore;  //把参数传入到成员变量里  
	
	
	
	FSlateFontInfo TitleTextFontInfo = FCoreStyle::GetDefaultFontStyle("Regular", 24);  //设置字体样式
	
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

			
		]
		
		//第三个槽位是资产列表
		+ SVerticalBox::Slot()
		.VAlign(VAlign_Fill)//垂直对齐-让槽位里的控件在垂直方向填满这个 Slot 给它的高度
		[

			SNew( SScrollBox)  // 滚动框控件


			+ SScrollBox::Slot() //给滚动框增加一个槽位
			[
				SNew(SListView<TSharedPtr<FAssetData>>)  //列表控件  里面是我们传入的资产数据类型
				.ItemHeight(24)  //设置每一行的高度
				.ListItemsSource(&StoredAssetsData)  //设置listview的  列表数据源  
				.OnGenerateRow(this,&ASdvanceDeletionTab::OnGenerateRowForList)  //设置生成行的回调函数  就是我们自己定义的函数  这个函数会在列表控件需要生成行的时候被调用
				
			 	
			]
			
		]
		
		//第四个槽来放三个按钮
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
		
			SNew( SHorizontalBox)  

			
		]
	
	];
	
	
}




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

TSharedRef<SCheckBox> ASdvanceDeletionTab::ConstructCheckBox(const TSharedPtr<FAssetData> AssetDataToDisplay)
{
	
	
	TSharedRef<SCheckBox> ConstructedCheckBox  =
	 SNew(SCheckBox)
	.Type(ESlateCheckBoxType::CheckBox) //  这里CheckBox的类型是复选框  还有切换按钮的类型
	.OnCheckStateChanged(this, &ASdvanceDeletionTab::OnCheckBoxStateChanges,AssetDataToDisplay) //依然绑定回调函数
	.Visibility(EVisibility::Visible); //可见性是可见
	
	return ConstructedCheckBox;
	
	
}

void ASdvanceDeletionTab::OnCheckBoxStateChanges(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData)
{
	
	switch (NewState)  //NewState 是枚举
	{
		case ECheckBoxState::Checked: //选中
		DebugHeader::Print(AssetData->AssetName.ToString(),FColor::Green);
			break;
		
		case ECheckBoxState::Unchecked: //未选中
		DebugHeader::Print(AssetData->AssetName.ToString(),FColor::Red);
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
	if (bAssetDeleted == false)
	{
		//刷新列表
	}
	
	
	
	return FReply::Handled();
	
}
