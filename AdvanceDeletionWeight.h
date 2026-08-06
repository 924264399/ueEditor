// Fill out your copyright notice in the Description page of Project Settings.
#pragma once


#include "Widgets/SCompoundWidget.h" //slat必须的类

class ASdvanceDeletionTab : public SCompoundWidget   //是 Slate 所有 "复合控件" 的基类，你写自定义 Slate 控件基本都继承它
{
	
	SLATE_BEGIN_ARGS(ASdvanceDeletionTab){ }


	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetsDataToStore)  //这个宏是声明参数的，，FString 是参数类型,TabName 是参数名

		
	SLATE_END_ARGS()   //这是一对宏  中间夹着的是参数声明，它们合起来会自动生成一个叫 FArguments 的内部结构体   简单说：这对宏 = 定义这个控件支持哪些外部参数
                        // 编译器看就是 struct FArguments{     }
						//以后可以用   SLATE_ATTRIBUTE 在中间声明参数
	
	
public:
	
	void Construct(const FArguments& InArgs);  //这可以理解为构造函数  
											   //const FArguments& InArgs 中的FArguments 就是上面的结构体来着		
	
	

	
private:
	TArray<TSharedPtr<FAssetData>> StoredAssetsData;  //这个是我们上面声明的参数类型    存储最原始的Fassetdata
	
	
	TArray<TSharedPtr<FAssetData>> DisplayAssetsData; //真正显示的Fassetdata数据  这样就能基于StoredAssetsData进行筛选
	
	TArray<TSharedRef<SCheckBox>> CheckBoxesArray; // 存储复选框的数组
	TArray<TSharedPtr<FAssetData>> AssetsDataToDeleteArray; //这个数组我们是希望当用户勾选的时候 把那个资产assetdata添加到这个数组
	
	TSharedRef<SListView<TSharedPtr<FAssetData>>> ConstructAssetListView();
	TSharedPtr<SListView<TSharedPtr<FAssetData>>>  ConstructedAssetListView;
	
	void RefreshAssetListView();
	
#pragma region 	ComboBoxForListingCondition
	
	TSharedRef<SComboBox<TSharedPtr<FString>>> ConstructComboBox();
	
	TArray< TSharedPtr<FString> > ComBoxSourceItems;
	
	TSharedRef<SWidget> OnGenerateComboContent(TSharedPtr<FString> SourceItem);
	
	void OnComboSelectionChanged( TSharedPtr<FString> SelectedOption,ESelectInfo::Type InSelectInfo);
	
	TSharedPtr<STextBlock> ComboDiplayTextBlock;
	
	TSharedRef<STextBlock> ConstructComboBoxWithHelpText(const FString& TextContent,ETextJustify::Type TextJustify );
	
	
	
#pragma endregion  	
	
	
#pragma region RowWidgetForAssetListView 	
	
	TSharedRef<ITableRow> OnGenerateRowForList(TSharedPtr<FAssetData> AssetDataToDisplay,   const TSharedRef<STableViewBase>& OwnerTable);  //  用来生成列表行的 
	
	
	void OnRowWidgetMoustButtonClicked(TSharedPtr<FAssetData> AssetDataToDisplay);
	
	
	TSharedRef<SCheckBox> ConstructCheckBox(const TSharedPtr<FAssetData> AssetDataToDisplay);  //  用来生成复选框的
	
	void OnCheckBoxStateChanges(ECheckBoxState NewState, TSharedPtr<FAssetData> AssetData);  //复选框状态改变的回调函数
	
	TSharedRef<STextBlock> ConstructTextForRowWidget(const FString& TextContent,const FSlateFontInfo& FontInfo);  //  用来生成文本控件的
	
	TSharedRef<SButton> ConstructButtonForRowWidget(const TSharedPtr<FAssetData> AssetDataToDisplay); //按钮控件
	
	FReply OnDeleteButttonClicked(TSharedPtr<FAssetData> AssetDataToDisplayData);
	
#pragma endregion	
	
	
#pragma region TabButtons
	
	TSharedRef<SButton> ConstructDeleteALLButton();
	TSharedRef<SButton> ConstructSelectedAllButton();
	TSharedRef<SButton> ConstructDeleteSelectedAllButton();
	
	
	FReply OnDeleteAllButtonClicked();
	FReply OnSelectedAllButtonClicked();
	FReply OnDeleteSelectedAllButtonClicked();
	
	TSharedRef<STextBlock> ConstructTextForTabButtons(const FString& TextContent);
	
#pragma endregion	
	
	
	
	
	FSlateFontInfo GetEmboseedTestFont() const { return FCoreStyle::Get().GetFontStyle(FName("EmbossedText")); };    //这是字体样式  
	
	
};
