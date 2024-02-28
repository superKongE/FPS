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


// �������� �ű涧
bool UWeaponSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UItemObject* DraggedItemObject = Cast<UItemObject>(InOperation->Payload);
	// �巡�׵Ȱ��� ���� ������Ʈ�� ���
	if (DraggedItemObject->GetIsSlotObject()) return false;

	if (ItemImage && DraggedItemObject && InventoryComponent)
	{
		// �ش� ���Կ� �̹� ���Ⱑ ����ִٸ�
		// ������ ���⸦ �κ��丮�� ����
		AddSlotToInventory();

		// �κ��丮 ���⸦ ���Կ� ����
		AddInventoryToSlot(DraggedItemObject);

		// ���Կ� �ڽ�(���� ����) �߰�
		AddWeaponWidgetToSlot(DraggedItemObject);

		// �巡�׵Ȱ��� ���� ������Ʈ�� ���
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
			// ���� ���н� ���� ����߸���
			InventoryComponent->FailSlotToInven(SlotIndex);
			//InventoryComponent->SpawnItem(SlotItemObject, true);
		}
		else if(CombatComponent)
		{
			// ������� ������ �ı�
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
		// ���� �����ϱ�
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

// ĳ���Ͱ� Ű�� ���� ���⸦ ������ ȣ���
void UWeaponSlotWidget::DropWeapon()
{
	RemoveItem();
}