#include "FPS/Widget/InventoryGridWidget.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Layout/Geometry.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "ToolDataVisualizer.h"
#include "Components/PanelWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Slate/SlateBrushAsset.h"

#include "FPS/ActorComponent/InventoryComponent.h"
#include "FPS/ActorComponent/CombatComponent.h"
#include "FPS/Item/ItemObject.h"
#include "FPS/Widget/ItemWidget.h"
#include "FPS/FPSCharacter.h"


UInventoryGridWidget::UInventoryGridWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	Brush = CreateDefaultSubobject<USlateBrushAsset>(TEXT("Brush"));
}
bool UInventoryGridWidget::Initialize()
{
	if (!Super::Initialize()) return false;

	if (GridBorder)
	{
		GridBorder->OnMouseButtonDownEvent.BindUFunction(this, FName("OnMouseButtonDown_Border"));
	}

	return true;
}
void UInventoryGridWidget::Init(UInventoryComponent* InventoryComponent_, float TileSize_)
{
	InventoryComponent = InventoryComponent_;
	TileSize = TileSize_;

	if (InventoryComponent)
	{
		UWidgetLayoutLibrary::SlotAsCanvasSlot(GridBorder)->SetSize(FVector2D(InventoryComponent->GetColums() * TileSize, InventoryComponent->GetRows() * TileSize));

		InventoryComponent->OnInventoryChanged.AddUObject(this, &UInventoryGridWidget::Refresh);
	}

	AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(InventoryComponent->GetOwner());
	if (FPSCharacter)
	{
		CombatComponent = FPSCharacter->GetCombatComponent();
	}

	CreateLineSegmenst();

	Refresh();
}
void UInventoryGridWidget::CreateLineSegmenst() const
{
	if (InventoryComponent == nullptr) return;
	
	CreateVerticleLines();
	CreateHorizontalLines();
}
void UInventoryGridWidget::CreateVerticleLines() const
{
	for (int32 i = 0; i < InventoryComponent->GetColums(); i++)
	{
		float X = i * TileSize;

		FLineStruct LineStruct;
		LineStruct.Start = FVector2D(X, 0.f);
		LineStruct.End = FVector2D(X, InventoryComponent->GetRows() * TileSize);
		Lines.Add(LineStruct);
	}
}
void UInventoryGridWidget::CreateHorizontalLines() const
{
	for (int32 i = 0; i < InventoryComponent->GetRows(); i++)
	{
		float Y = i * 50;

		FLineStruct LineStruct;
		LineStruct.Start = FVector2D(0.f, Y);
		LineStruct.End = FVector2D(InventoryComponent->GetColums() * TileSize, Y);
		Lines.Add(LineStruct);
	}
}



void UInventoryGridWidget::Refresh()
{
	if (GridCanvasPanel && InventoryComponent)
	{
		GridCanvasPanel->ClearChildren();	

		APlayerController* OwningPlayerController = Cast<APlayerController>(GetOwningPlayerPawn()->GetController());
		if (OwningPlayerController == nullptr || ItemWidgetClass == nullptr) return;

		TMap<UItemObject*, FIndexArray>& ItempMaps = InventoryComponent->GetAllItems();
		for (auto& data : ItempMaps)
		{
			UItemWidget* ItemWidget = CreateWidget<UItemWidget>(OwningPlayerController, ItemWidgetClass);
			if (ItemWidget)
			{
				ItemWidget->Init(TileSize, data.Key, false);
				ItemWidget->ItemRemoved_OneParam.BindUObject(this, &UInventoryGridWidget::RemoveItem);

				if (GridCanvasPanel)
				{
					UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(GridCanvasPanel->AddChild(ItemWidget));
					if (CanvasPanelSlot)
					{
						FTileStruct TileStruct;
						TileStruct.X = data.Value.IndexArray[0] % InventoryComponent->GetColums();
						TileStruct.Y = data.Value.IndexArray[0] / InventoryComponent->GetColums();

						CanvasPanelSlot->SetAutoSize(true);
						CanvasPanelSlot->SetPosition(FVector2D(TileStruct.X * TileSize, TileStruct.Y * TileSize));
					}
				}
			}
		}
	}
}
// UItemWidget�� �巡�׵Ǹ� ȣ��Ǵ� �Լ�
void UInventoryGridWidget::RemoveItem(class UItemObject* ItemObject)
{
	if (InventoryComponent)
	{
		InventoryComponent->RemoveItem(ItemObject);
	}
}



FEventReply UInventoryGridWidget::OnMouseButtonDown_Border(FGeometry MyGeometry, const FPointerEvent& MouseEvent)
{
	return FEventReply(true);
}


void UInventoryGridWidget::MousePositionInTile(FVector2D MousePosition, bool& bRight, bool& bDown)
{
	float temp;
	UKismetMathLibrary::FMod(MousePosition.X, TileSize, temp);
	bRight = temp > (TileSize / 2.f);


	UKismetMathLibrary::FMod(MousePosition.Y, TileSize, temp);
	bDown = temp > (TileSize / 2.f);
}


int32 UInventoryGridWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FPaintContext PaintContext(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	for (auto& Line : Lines)
	{
		FVector2D LocalTopLeft = USlateBlueprintLibrary::GetLocalTopLeft(GridBorder->GetCachedGeometry());
		FVector2D PositionA = Line.Start + LocalTopLeft;
		FVector2D PositionB = Line.End + LocalTopLeft;

		UWidgetBlueprintLibrary::DrawLine(PaintContext, PositionA, PositionB, FLinearColor(0.5f, 0.5f, 0.5f, 0.5f));
	}

	if (UWidgetBlueprintLibrary::IsDragDropping() && bDrawDropLocation)
	{
		UItemObject* ItemObject = Cast<UItemObject>(UWidgetBlueprintLibrary::GetDragDroppingContent()->Payload);
		if (ItemObject && InventoryComponent)
		{
			int32 TopLeftIndex = InventoryComponent->TileToIndex(FTileStruct(DragTopLeftIndex.X, DragTopLeftIndex.Y));
			FTileStruct Tile = InventoryComponent->IndexToTile(TopLeftIndex);

			FVector2D Position(Tile.X * TileSize, Tile.Y * TileSize);
			FVector2D Size(ItemObject->GetDimensions().X * TileSize, ItemObject->GetDimensions().Y * TileSize);
			if (InventoryComponent->IsRoomAvailable(ItemObject, TopLeftIndex))
			{
				if(Brush)
					UWidgetBlueprintLibrary::DrawBox(PaintContext, Position, Size, Brush, FLinearColor(0.f, 1.f, 0.f, 0.25f));
			}
			else
			{
				if(Brush)
					UWidgetBlueprintLibrary::DrawBox(PaintContext, Position, Size, Brush, FLinearColor(1.f, 0.f, 0.f, 0.25f));
			}
		}
	}

	return LayerId;
}


// �� ���� ������ ó�� �巡�׵Ǵ� �������� ȣ���
void UInventoryGridWidget::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UItemObject* ItemObject = Cast<UItemObject>(UWidgetBlueprintLibrary::GetDragDroppingContent()->Payload);
	if (ItemObject && InventoryComponent)
	{
		TMap<UItemObject*, FIndexArray>& ItemMaps = InventoryComponent->GetAllItems();
		FIndexArray* IndexArrayStruct = ItemMaps.Find(ItemObject);
		if (IndexArrayStruct)
		{
			for (int32 Index : IndexArrayStruct->IndexArray)
			{
				InventoryComponent->SetItemArray(Index, false);
			}
		}
	}

	bDrawDropLocation = true;
}
void UInventoryGridWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	bDrawDropLocation = false;
}


// ���� �� ���� ������ �巡�����̸� ȣ��Ǵ� �Լ�
bool UInventoryGridWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// ���� ���� �������� ��ǥ
	DragTopLeftIndex = USlateBlueprintLibrary::AbsoluteToLocal(InGeometry, InDragDropEvent.GetScreenSpacePosition());

	bool bRight, bDown;
	MousePositionInTile(DragTopLeftIndex, bRight, bDown);

	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	int32 X = FMath::Clamp(ItemObject->GetDimensions().X - bRight, 0, ItemObject->GetDimensions().X - bRight);
	int32 Y = FMath::Clamp(ItemObject->GetDimensions().Y - bDown, 0, ItemObject->GetDimensions().Y - bDown);
	

	DragTopLeftIndex /= TileSize; // ���콺 ��ġ�� �ε���
	DragTopLeftIndex -= FVector2D(X / 2, Y / 2); // ���콺�� ���� �߾ӿ� ��ġ�ϹǷ� ���ݸ�ŭ �������� �̵��Ѱ��� TopLeftIndex

	return true;
}
// �巡�� ��
bool UInventoryGridWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// gridwidget�� ���������� inventorywidget������ nativeondrop�� ȣ��Ǵ°��� ���� ����
	//InventoryComponent->SetDropEventHandled(true);

	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	if (ItemObject == nullptr || InventoryComponent == nullptr) return false;

	// ���Կ��� �κ��丮�� �ű�� ���
	if (ItemObject->GetIsSlotObject())
	{
		// �κ��� ���� ������
		if (InventoryComponent->TryAddItem(ItemObject))
		{
			InventoryComponent->DeleteFromSlot(ItemObject, false);
		}
		else
		{
			InventoryComponent->DeleteFromSlot(ItemObject, true);
		}
	}
	// �κ����� �κ�����
	else
	{
		// ���� ������
		if (!IsRoomAvailableForPayload(ItemObject))
		{
			// ������ ������
			InventoryComponent->DeleteFormInventory(ItemObject);
		}
	}

	return true;
}
bool UInventoryGridWidget::IsRoomAvailableForPayload(class UItemObject* Payload)
{
	if (Payload == nullptr || InventoryComponent == nullptr) return false;

	int32 TopLeftIndex = InventoryComponent->TileToIndex(FTileStruct(DragTopLeftIndex.X, DragTopLeftIndex.Y));
	// �ش� ��ġ�� ������ �ִ���
	if (InventoryComponent->IsRoomAvailable(Payload, TopLeftIndex))
	{
		InventoryComponent->AddItem(Payload, TopLeftIndex);
		return true;
	}
	// �ش� ������ �������� ������ ã�Ƴֱ�
	else
	{
		if (InventoryComponent->TryAddItem(Payload))
			return true;
	}

	return false;
}


// �ش� ������ focus�� ���¿��� Ű�� ���� ��� ȣ���
FReply UInventoryGridWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == FKey("R") && UWidgetBlueprintLibrary::GetDragDroppingContent())
	{
		UItemObject* ItemObject = Cast<UItemObject>(UWidgetBlueprintLibrary::GetDragDroppingContent()->Payload);
		UItemWidget* ItemWidget = Cast<UItemWidget>(UWidgetBlueprintLibrary::GetDragDroppingContent()->DefaultDragVisual);
		if (ItemObject && ItemWidget)
		{
			ItemObject->Rotate();
			ItemWidget->Refresh();
		}
	}

	return FReply::Handled();
}