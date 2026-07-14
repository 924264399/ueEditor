// Fill out your copyright notice in the Description page of Project Settings.
#pragma once


#include "Widgets/SCompoundWidget.h" //slat必须的类

class ASdvanceDeletionTab : public SCompoundWidget   //是 Slate 所有 "复合控件" 的基类，你写自定义 Slate 控件基本都继承它
{
	
	SLATE_BEGIN_ARGS(ASdvanceDeletionTab){ }


	SLATE_ARGUMENT(TArray<TSharedPtr<FAssetData>>, AssetsDataArray)  //这个宏是声明参数的，，FString 是参数类型,TabName 是参数名

		
	SLATE_END_ARGS()   //这是一对宏  中间夹着的是参数声明，它们合起来会自动生成一个叫 FArguments 的内部结构体   简单说：这对宏 = 定义这个控件支持哪些外部参数
                        // 编译器看就是 struct FArguments{     }
						//以后可以用   SLATE_ATTRIBUTE 在中间声明参数
	
	
public:
	
	void Construct(const FArguments& InArgs);  //这可以理解为构造函数  
											   //const FArguments& InArgs 中的FArguments 就是上面的结构体来着		
	
	

	
private:
	TArray<TSharedPtr<FAssetData>> AssetsDataArray;  //这个是我们上面声明的参数类型  这个是我们自己定义的成员变量  用来存储传入的参数数
	
	
};
