#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponSlotWidget.generated.h"


UCLASS()
class FPS_API UWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	bool Initialize() override;
	void Init(class UInventoryComponent* InventoryComponent_, int32 Index_);

	UFUNCTION()
	FEventReply OnMouseButtonDown_Border(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);

	UFUNCTION()
	void RemoveItem();

	void EquipWeapon();
	UFUNCTION()
	void DropWeapon();

	void AddSlotToInventory();
	void AddInventoryToSlot(class UItemObject* ItemObject);
	void AddWeaponWidgetToSlot(class UItemObject* ItemObject);
	
private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	class UBorder* BackgroundBorder = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	class UEditableTextBox* ItemName = nullptr;

	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	class UImage* ItemImage = nullptr;

private:
	UPROPERTY()
	class UCombatComponent* CombatComponent = nullptr;
	UPROPERTY()
	class UInventoryComponent* InventoryComponent = nullptr;

	bool bUsing = false;

	int32 SlotIndex;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UDragDropOperation> DragDropOperationClass;
	UPROPERTY(EditAnywhere, Category = Item)
	TSubclassOf<class UItemWidget> ItemWidgetClass;
};
