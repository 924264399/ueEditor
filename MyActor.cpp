// Fill out your copyright notice in the Description page of Project Settings.


#include "MyActor.h"

// Sets default values
AMyActor::AMyActor()
{
	CaptureTerrain = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("CaptureTerrain"));

	BoxComponent =  CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	

}




