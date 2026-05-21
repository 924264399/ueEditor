// Fill out your copyright notice in the Description page of Project Settings.


#include "quickassetactor/quickassetactor.h"


void Uquickassetactor::TestFunction()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,8.f, FColor::Green,TEXT("Working"));
		
	}
}

