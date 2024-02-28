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
// UItemWidget이 드래그되면 호출되는 함수
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


// 이 위젯 위에서 처음 드래그되는 순간마다 호출됨
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


// 현재 이 위젯 위에서 드래그중이면 호출되는 함수
bool UInventoryGridWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// 현재 위젯 내에서의 좌표
	DragTopLeftIndex = USlateBlueprintLibrary::AbsoluteToLocal(InGeometry, InDragDropEvent.GetScreenSpacePosition());

	bool bRight, bDown;
	MousePositionInTile(DragTopLeftIndex, bRight, bDown);

	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	int32 X = FMath::Clamp(ItemObject->GetDimensions().X - bRight, 0, ItemObject->GetDimensions().X - bRight);
	int32 Y = FMath::Clamp(ItemObject->GetDimensions().Y - bDown, 0, ItemObject->GetDimensions().Y - bDown);
	

	DragTopLeftIndex /= TileSize; // 마우스 위치의 인덱스
	DragTopLeftIndex -= FVector2D(X / 2, Y / 2); // 마우스가 위젯 중앙에 위치하므로 절반만큼 왼쪽으로 이동한것이 TopLeftIndex

	return true;
}
// 드래그 땜
bool UInventoryGridWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	// gridwidget의 상위위젯인 inventorywidget에서도 nativeondrop이 호출되는것을 막기 위함
	//InventoryComponent->SetDropEventHandled(true);

	UItemObject* ItemObject = Cast<UItemObject>(InOperation->Payload);
	if (ItemObject == nullptr || InventoryComponent == nullptr) return false;

	// 슬롯에서 인벤토리로 옮기는 경우
	if (ItemObject->GetIsSlotObject())
	{
		// 인벤에 삽입 성공시
		if (InventoryComponent->TryAddItem(ItemObject))
		{
			InventoryComponent->DeleteFromSlot(ItemObject, false);
		}
		else
		{
			InventoryComponent->DeleteFromSlot(ItemObject, true);
		}
	}
	// 인벤에서 인벤으로
	else
	{
		// 공간 부족시
		if (!IsRoomAvailableForPayload(ItemObject))
		{
			// 생성후 버리기
			InventoryComponent->DeleteFormInventory(ItemObject);
		}
	}

	return true;
}
bool UInventoryGridWidget::IsRoomAvailableForPayload(class UItemObject* Payload)
{
	if (Payload == nullptr || InventoryComponent == nullptr) return false;

	int32 TopLeftIndex = InventoryComponent->TileToIndex(FTileStruct(DragTopLeftIndex.X, DragTopLeftIndex.Y));
	// 해당 위치에 공간이 있는지
	if (InventoryComponent->IsRoomAvailable(Payload, TopLeftIndex))
	{
		InventoryComponent->AddItem(Payload, TopLeftIndex);
		return true;
	}
	// 해당 공간이 차있으면 공간을 찾아넣기
	else
	{
		if (InventoryComponent->TryAddItem(Payload))
			return true;
	}

	return false;
}


// 해당 위젯이 focus된 상태에서 키를 누를 경우 호출됨
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