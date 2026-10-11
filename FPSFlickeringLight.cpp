#include "FPS/FPSFlickeringLight.h"

#include "Components/PointLightComponent.h"
#include "Engine/World.h"

AFPSFlickeringLight::AFPSFlickeringLight()
{
	PrimaryActorTick.bCanEverTick = false;

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	RootComponent = PointLight;
}

void AFPSFlickeringLight::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(PointLight))
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no valid PointLight component."), *GetName());
		return;
	}

	CachedNormalIntensity = NormalIntensity > 0.0f
		? NormalIntensity
		: PointLight->Intensity;

	if (CachedNormalIntensity <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no positive normal intensity."), *GetName());
		return;
	}

	PointLight->SetIntensity(CachedNormalIntensity);

	if (bAutoStart)
	{
		StartFlickering();
	}
}

void AFPSFlickeringLight::StartFlickering()
{
	if (!IsValid(PointLight) || !GetWorld())
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(FlickerTimerHandle);
	ScheduleNextFlicker();
}

void AFPSFlickeringLight::StopFlickering()
{
	GetWorldTimerManager().ClearTimer(FlickerTimerHandle);

	if (IsValid(PointLight) && CachedNormalIntensity > 0.0f)
	{
		PointLight->SetIntensity(CachedNormalIntensity);
	}
}

void AFPSFlickeringLight::ApplyNextFlicker()
{
	if (!IsValid(PointLight))
	{
		return;
	}

	const float LowerMultiplier = FMath::Min(MinFlickerMultiplier, MaxFlickerMultiplier);
	const float UpperMultiplier = FMath::Max(MinFlickerMultiplier, MaxFlickerMultiplier);
	const float RandomMultiplier = FMath::FRandRange(LowerMultiplier, UpperMultiplier);
	const float RandomIntensity = CachedNormalIntensity * RandomMultiplier;

	PointLight->SetIntensity(RandomIntensity);
	ScheduleNextFlicker();
}

void AFPSFlickeringLight::ScheduleNextFlicker()
{
	if (!GetWorld())
	{
		return;
	}

	const float MinTime = FMath::Max(0.01f, FMath::Min(MinFlickerTime, MaxFlickerTime));
	const float MaxTime = FMath::Max(MinTime, FMath::Max(MinFlickerTime, MaxFlickerTime));
	const float Delay = FMath::FRandRange(MinTime, MaxTime); //在最大间隔时间 和  最小间隔时间 之间随机  可靠度更高

	GetWorldTimerManager().SetTimer(
		FlickerTimerHandle,
		this,
		&AFPSFlickeringLight::ApplyNextFlicker,
		Delay,
		false);    //然后基于随机时间调用ApplyNextFlicker
}
