#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture2D.h"
#include "CharacterCapture2D.generated.h"


UCLASS()
class FPS_API ACharacterCapture2D : public ASceneCapture2D
{
	GENERATED_BODY()
	
public:
	ACharacterCapture2D(const FObjectInitializer& ObjectInitializer);

	void Init(class ARenderTargetCharacter* RenderCharacter);
};
