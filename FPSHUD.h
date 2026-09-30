#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FPSHUD.generated.h"

UCLASS()
class CCSTUDY_API AFPSHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
