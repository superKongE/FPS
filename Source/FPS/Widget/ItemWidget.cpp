#include "FPS/Widget/ItemWidget.h"
#include "Components/SizeBox.h"
#include "Components/Image.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Border.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Materials/MaterialInterface.h"
#include "Blueprint/DragDropOperation.h"

#include "FPS/Item/ItemObject.h"
#include "FPS/Widget/WeaponSlotWidget.h"


void UItemWidget::Init(float TileSize_, class UItemObject* ItemObject_, bool bSlotWidget_)
{
	TileSize = TileSize_;
	ItemObject = ItemObject_;
	bSlotWidget = bSlotWidget_;

	SetIconImage();

	Refresh();
}


void UItemWidget::Refresh()
{
	SetIconImage();

	FIntPoint Dimensions;
	if (ItemObject && !ItemObject->GetIsSlotObject())
	{
		Dimensions = ItemObject->GetDimensions();
	}
	else if (ItemObject)
	{
		Dimensions = ItemObject->GetUnRotateDimensions();
	}

	Size.X = Dimensions.X * TileSize;
	Size.Y = Dimensions.Y * TileSize;

	if (BackGroundSizeBox)
	{
		BackGroundSizeBox->SetWidthOverride(Size.X);
		BackGroundSizeBox->SetHeightOverride(Size.Y);
	}

	if (ItemImage)
	{
		UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ItemImage);
		CanvasPanelSlot->SetSize(Size);
	}
}


void UItemWidget::SetIconImage()
{
	if (ItemObject == nullptr) return;

	UMaterialInstance* IconMaterialInstance = nullptr;
	if (!ItemObject->GetIsSlotObject())
		IconMaterialInstance = ItemObject->GetIcon();
	else
	{
		IconMaterialInstance = ItemObject->GetItemIcon();
	}

	if (IconMaterialInstance)
	{
		FSlateBrush ItemBrush = UWidgetBlueprintLibrary::MakeBrushFromMaterial(IconMaterialInstance, Size.X, Size.Y);
		ItemImage->SetBrush(ItemBrush);
	}
}


// ���콺�� �ö󰡸�
void UItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (BackGroundBorder)
	{
		BackGroundBorder->SetBrushColor(FLinearColor(0.5f, 0.5f, 0.5f, 0.2f));
	}
}
// ��������
void UItemWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (BackGroundBorder)
	{
		BackGroundBorder->SetBrushColor(FLinearColor(0.f, 0.f, 0.f, 0.5f));
	}
}


FReply UItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}
void UItemWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (DragDropOperationClass)
	{
		OutOperation = UWidgetBlueprintLibrary::CreateDragDropOperation(DragDropOperationClass);	
		OutOperation->Payload = ItemObject; // �巡���ؼ� ���� �ٴ� ������Ʈ(������)
		OutOperation->DefaultDragVisual = this; // �巡�׵Ǵ� ���� ǥ�õ� ������Ʈ
		OutOperation->Pivot = EDragPivot::CenterCenter;
		OutOperation->Offset = FVector2D(0.f, 0.f);
	}

	if (!bSlotWidget)
		ItemRemoved_OneParam.Execute(ItemObject);
	else
		ItemRemoved.Execute();

	RemoveFromParent();
}