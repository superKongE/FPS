#include "FPS/SceneCapture/CharacterCapture2D.h"
#include "Components/SceneCaptureComponent2D.h"

#include "FPS/FPSCharacter.h"
#include "FPS/RenderTargetCharacter.h"


ACharacterCapture2D::ACharacterCapture2D(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	GetCaptureComponent2D()->ProjectionType = ECameraProjectionMode::Orthographic;
	GetCaptureComponent2D()->OrthoWidth = 120.f;
	GetCaptureComponent2D()->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
	GetCaptureComponent2D()->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
}

void ACharacterCapture2D::Init(ARenderTargetCharacter* RenderCharacter)
{
	if (RenderCharacter == nullptr) return;

	
	SetOwner(RenderCharacter);
	TArray<TObjectPtr<AActor>> ActorsToCapture;
	ActorsToCapture.Add(TObjectPtr<AActor>(RenderCharacter));

	GetCaptureComponent2D()->ShowOnlyActors = ActorsToCapture;
}