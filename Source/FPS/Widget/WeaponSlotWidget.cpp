#include "FPS/Widget/WeaponSlotWidget.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/EditableTextBox.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/CanvasPanelSlot.h"

#include "FPS/Item/ItemObject.h"
#include "FPS/ActorComponent/InventoryComponent.h"
#include "FPS/Widget/ItemWidget.h"
#include "FPS/FPSCharacter.h"
#include "FPS/ActorComponent/CombatComponent.h"


bool UWeaponSlotWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	if (BackgroundBorder)
	{
		BackgroundBorder->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDown_Border"));
	}

	return true;
}
void UWeaponSlotWidget::Init(UInventoryComponent* InventoryComponent_, int32 Index_)
{
	InventoryComponent = InventoryComponent_;
	SlotIndex = Index_;

	AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(InventoryComponent->GetOwner());
	if (FPSCharacter)
	{
		CombatComponent = FPSCharacter->GetCombatComponent();
		if (CombatComponent)
		{
			if (SlotIndex == 0)
				CombatComponent->FirstSlotWeaponDrop.BindUFunction(this, FName("DropWeapon"));
			else if(SlotIndex == 1)
				CombatComponent->SecondSlotWeaponDrop.BindUFunction(this, FName("DropWeapon"));
			else if(SlotIndex == 2)
				CombatComponent->GrenadeSlotDrop.BindUFunction(this, FName("DropWeapon"));
		}
	}
}


FEventReply UWeaponSlotWidget::OnMouseButtonDown_Border(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	return FEventReply(true);
}


// 슬롯으로 옮길때
bool UWeaponSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UItemObject* DraggedItemObject = Cast<UItemObject>(InOperation->Payload);
	// 드래그된것이 슬롯 오브젝트인 경우
	if (DraggedItemObject->GetIsSlotObject()) return false;

	if (ItemImage && DraggedItemObject && InventoryComponent)
	{
		// 해당 슬롯에 이미 무기가 들어있다면
		// 슬롯의 무기를 인벤토리에 삽입
		AddSlotToInventory();

		// 인벤토리 무기를 슬롯에 삽입
		AddInventoryToSlot(DraggedItemObject);

		// 슬롯에 자식(무기 위젯) 추가
		AddWeaponWidgetToSlot(DraggedItemObject);

		// 드래그된것이 슬롯 오브젝트인 경우
		/*if (DraggedItemObject->GetIsSlotObject())
		{

		}*/

		return true;
	}

	return true;
}
void UWeaponSlotWidget::AddSlotToInventory()
{
	UItemObject* SlotItemObject = InventoryComponent->GetSlotItemObject(SlotIndex);
	if (SlotItemObject)
	{
		SlotItemObject->SetIsSlotObject(false);
		InventoryComponent->SetSlotItemObject(nullptr, SlotIndex);
		if (!InventoryComponent->TryAddItem(SlotItemObject))
		{
			// 삽입 실패시 무기 떨어뜨리기
			InventoryComponent->FailSlotToInven(SlotIndex);
			//InventoryComponent->SpawnItem(SlotItemObject, true);
		}
		else if(CombatComponent)
		{
			// 생성됬던 아이템 파괴
			InventoryComponent->SlotToInven(SlotIndex);
		}
	}
}
void UWeaponSlotWidget::AddInventoryToSlot(UItemObject* ItemObject)
{
	ItemObject->SetIsSlotObject(true);
	InventoryComponent->SetSlotItemObject(ItemObject, SlotIndex);
	AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(GetOwningPlayerPawn());
	if (FPSCharacter)
	{
		// 무기 장착하기
		FPSCharacter->EquipWeaponFromSlot(ItemObject, SlotIndex);
	}
}
void UWeaponSlotWidget::AddWeaponWidgetToSlot(UItemObject* ItemObject)
{
	if (BackgroundBorder)
		BackgroundBorder->ClearChildren();

	if (ItemName)
	{
		ItemName->SetText(FText::FromName(ItemObject->GetItemName()));
	}

	APlayerController* OwningPlayerController = Cast<APlayerController>(GetOwningPlayerPawn()->GetController());
	UItemWidget* ItemWidget = CreateWidget<UItemWidget>(OwningPlayerController, ItemWidgetClass);
	if (ItemWidget)
	{
		ItemWidget->Init(50, ItemObject, true);
		ItemWidget->ItemRemoved.BindUObject(this, &UWeaponSlotWidget::RemoveItem);

		if (BackgroundBorder)
		{
			UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(BackgroundBorder->AddChild(ItemWidget));
			if (CanvasPanelSlot)
			{
				CanvasPanelSlot->SetAutoSize(false);
				CanvasPanelSlot->SetPosition(FVector2D(0.f, 0.f));
			}
		}
	}
}


void UWeaponSlotWidget::RemoveItem()
{
	if(InventoryComponent)
		InventoryComponent->SetSlotItemObject(nullptr, SlotIndex);

	if (BackgroundBorder)
		BackgroundBorder->ClearChildren();
}


void UWeaponSlotWidget::EquipWeapon()
{

}

// 캐릭터가 키를 눌러 무기를 버리면 호출됨
void UWeaponSlotWidget::DropWeapon()
{
	RemoveItem();
}