#include "FPS/Widget/CharacterPreviewWidget.h"
#include "Components/Image.h"
#include "Components/Border.h"

#include "FPS/FPSCharacter.h"
#include "FPS/RenderTargetCharacter.h"

bool UCharacterPreviewWidget::Initialize()
{
	if (!Super::Initialize()) return false;
	UE_LOG(LogTemp, Error, TEXT("Initialize"));
	if (CharacterPreviewImage && RenderTarget)
	{
		CharacterPreviewImage->SetBrushFromMaterial(RenderTarget);
	}

	if (GetOwningPlayerPawn())
	{
		OwnerCharacter = Cast<AFPSCharacter>(GetOwningPlayerPawn());
	}

	if (CharacterPreviewBorder && CharacterPreviewImage)
	{
		CharacterPreviewBorder->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDown_Border"));
		CharacterPreviewImage->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDown_Border"));
	}

	return true;
}


FReply UCharacterPreviewWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bMouseClicked = true;
	CurrentMouseLocation = InMouseEvent.GetScreenSpacePosition();
	
	return FReply::Handled();
}
FReply UCharacterPreviewWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(OwnerCharacter == nullptr || !bMouseClicked) return FReply::Handled();

	PrevMouseLocation = CurrentMouseLocation;
	CurrentMouseLocation = InMouseEvent.GetScreenSpacePosition();
	if (CurrentMouseLocation.X - PrevMouseLocation.X > 0.f)
	{
		OwnerCharacter->AddRotationYaw(5.f);
	}
	else if(CurrentMouseLocation.X - PrevMouseLocation.X < 0.f)
	{
		OwnerCharacter->AddRotationYaw(-5.f);
	}

	return FReply::Handled();
}
FReply UCharacterPreviewWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bMouseClicked = false;
	return FReply::Handled();
}
void UCharacterPreviewWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	bMouseClicked = false;
}