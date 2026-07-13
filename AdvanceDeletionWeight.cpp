// Fill out your copyright notice in the Description page of Project Settings.


#include "SlateWights/AdvanceDeletionWeight.h"


void ASdvanceDeletionTab::Construct(const FArguments& InArgs)
{
	bCanSupportFocus = true;  //是 SWidget 基类里的一个布尔成员变量，表示这个控件能不能获得键盘焦点（ 即能接受键盘输入  激活些什么）
	
	
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
		SNew(SVerticalBox)  //防止垂直盒子控件 里面可以放多个槽位  这样我们才能放入更多的东西比如多个文本

		//标题文本的第一个垂直槽
		+ SVerticalBox::Slot()  //用这个来添加槽位
		.AutoHeight()  // 设置槽位属性 这里是自动高度
		[
			SNew(STextBlock) //在槽位里创建一个文本块
			.Text(FText::FromString(InArgs._TestString))
			.Font(TitleTextFontInfo)  //设置字体样式
			.Justification(ETextJustify::Center)//居中对其
			.ColorAndOpacity(FColor::Blue) //设置颜色
		]
		
	
	];
	
	
}
