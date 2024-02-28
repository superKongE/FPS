#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterPreviewWidget.generated.h"


UCLASS()
class FPS_API UCharacterPreviewWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	bool Initialize() override;

	FEventReply OnMouseButtonDown_Border(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent);

private:
	UPROPERTY(BlueprintReadWrite, meta=(BindWidget, AllowPrivateAccess="true"))
	class UBorder* CharacterPreviewBorder = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	class UImage* CharacterPreviewImage = nullptr;

	UPROPERTY(EditAnywhere)
	class UMaterialInterface* RenderTarget = nullptr;

	UPROPERTY()
	class AFPSCharacter* OwnerCharacter = nullptr;

	FVector2D CurrentMouseLocation;
	FVector2D PrevMouseLocation;

	bool bMouseClicked = false;
};
