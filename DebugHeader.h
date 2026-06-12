#pragma once

#include "Misc/MessageDialog.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"

void Print(const FString& Message,const FColor& Color)  //加const 只读保护 函数内不更改   &是引用取别名
{
	
	if (GEngine) //使用指针前先判空，这是防御性编程，避免极端场景闪退。
	{
		GEngine->AddOnScreenDebugMessage(-1,8.f,Color,Message);
		
	}
	
	
}



void PrintLog(const FString& Message)
{
	
	UE_LOG(LogTemp, Warning, TEXT("%s"),*Message);  //这个是调试信息打印
	
}



// 通用弹窗工具函数
// MsgType: 按钮类型（Ok/YesNo/OkCancel等）
// Message: 弹窗内容（传入FString即可）
// bShowMsgAsWarning: 是否显示为警告弹窗（带"Warning"标题）
EAppReturnType::Type ShowMsgDialog(EAppMsgType::Type MsgType, const FString& Message, bool bShowMsgAsWarning = true)
{
	if(bShowMsgAsWarning)
	{
		// 警告弹窗，标题固定为"Warning"
		const FText MsgTitle = FText::FromString(TEXT("Warning"));
		const FText MsgContent = FText::FromString(Message);
		return FMessageDialog::Open(MsgType, MsgContent, MsgTitle);
	}
	else
	{
		// 普通弹窗，使用默认标题
		return FMessageDialog::Open(MsgType, FText::FromString(Message));
	}
}


// 编辑器通知工具函数
// Message: 要显示的通知内容（传入FString即可）
void ShowNotifyInfo(const FString& Message)
{
	// 1. 创建通知配置对象，传入显示文本（FString → FText）  实例化+构造
	FNotificationInfo NotifyInfo(FText::FromString(Message)); 
    
	// 2. 配置通知样式
	NotifyInfo.bUseLargeFont = true;    // 使用大号字体，更醒目
	NotifyInfo.FadeOutDuration = 7.0f;  // 通知7秒后自动淡出消失

	// 3. 获取通知管理器单例，添加通知并弹出
	FSlateNotificationManager::Get().AddNotification(NotifyInfo);
}


