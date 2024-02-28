#include "FPS/Widget/InventoryWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/VerticalBox.h"
#include "Blueprint/DragDropOperation.h"

#include "FPS/Widget/InventoryGridWidget.h"
#include "FPS/ActorComponent/InventoryComponent.h"
#include "FPS/FPSCharacter.h"
#include "FPS/Item/Item.h"
#include "FPS/Item/ItemObject.h"
#include "FPS/Widget/WeaponSlotWidget.h"


bool UInventoryWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	if (BackgroundBorder)
	{
		BackgroundBorder->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDown_Border"));
	}

	return true;
}
void UInventoryWidget::Init(UInventoryComponent* InventoryComponent_, float TileSize_)
{
	InventoryComponent = InventoryComponent_;
	TileSize = TileSize_;

	if(InventoryGridWidget && InventoryComponent)
		InventoryGridWidget->Init(InventoryComponent, TileSize_);

	APlayerController* OwningPlayerController = Cast<APlayerController>(GetOwningPlayerPawn()->GetController());
	if (WeapomVerticalBox && OwningPlayerController && WeaponSlotWidgetClass)
	{
		UWeaponSlotWidget* FirstWeaponSlotWidget = CreateWidget<UWeaponSlotWidget>(OwningPlayerController, WeaponSlotWidgetClass);
		UWeaponSlotWidget* SecondWeaponSlotWidget = CreateWidget<UWeaponSlotWidget>(OwningPlayerController, WeaponSlotWidgetClass);

		if (FirstWeaponSlotWidget && SecondWeaponSlotWidget)
		{
			FirstWeaponSlotWidget->Init(InventoryComponent, 0);
			SecondWeaponSlotWidget->Init(InventoryComponent, 1);

			WeapomVerticalBox->AddChild(FirstWeaponSlotWidget);
			WeapomVerticalBox->AddChild(SecondWeaponSlotWidget);
		}
	}
}


FEventReply UInventoryWidget::OnMouseButtonDown_Border(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	return FEventReply(true);
}


bool UInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	if (ItemObject)
	{
		if (ItemObject->GetIsSlotObject())
		{
			// 슬롯에서 버리는 경우(생성된 총을 버림)
			InventoryComponent->DeleteFromSlot(ItemObject, true);
		}
		else
		{
			// 인벤토리에서 버리는 경우 (총을 생성후 버림)
			InventoryComponent->DeleteFormInventory(ItemObject);
		}
	}

	return true;
}