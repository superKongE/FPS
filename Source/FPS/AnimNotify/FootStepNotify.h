#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FootStepNotify.generated.h"


UCLASS()
class FPS_API UFootStepNotify : public UAnimNotify
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess="true"))
	FName FootName;
};
