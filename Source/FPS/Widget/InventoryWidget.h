#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

/**
 * 
 */
UCLASS()
class FPS_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	bool Initialize() override;

	void Init(class UInventoryComponent* InventoryComponent_, float TileSize_);

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);

	UFUNCTION()
	FEventReply OnMouseButtonDown_Border(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

private:
	UPROPERTY()
	class UInventoryComponent* InventoryComponent = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	class UInventoryGridWidget* InventoryGridWidget = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	class UBorder* BackgroundBorder = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	class UVerticalBox* WeapomVerticalBox = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UWeaponSlotWidget> WeaponSlotWidgetClass;

	float TileSize;
};
