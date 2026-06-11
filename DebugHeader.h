#pragma once

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



