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


// 마우스가 올라가면
void UItemWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (BackGroundBorder)
	{
		BackGroundBorder->SetBrushColor(FLinearColor(0.5f, 0.5f, 0.5f, 0.2f));
	}
}
// 내려가면
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
		OutOperation->Payload = ItemObject; // 드래그해서 끌고 다닐 오브젝트(데이터)
		OutOperation->DefaultDragVisual = this; // 드래그되는 동안 표시될 오브젝트
		OutOperation->Pivot = EDragPivot::CenterCenter;
		OutOperation->Offset = FVector2D(0.f, 0.f);
	}

	if (!bSlotWidget)
		ItemRemoved_OneParam.Execute(ItemObject);
	else
		ItemRemoved.Execute();

	RemoveFromParent();
}