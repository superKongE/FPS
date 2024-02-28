#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FPS/Struct/LineStruct.h"
#include "InventoryGridWidget.generated.h"


UCLASS()
class FPS_API UInventoryGridWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UInventoryGridWidget(const FObjectInitializer& ObjectInitializer);
	bool Initialize() override;
	void Init(class UInventoryComponent* InventoryComponent_, float TileSize_);
	UFUNCTION(BlueprintCallable)
	void CreateLineSegmenst() const;
	void CreateVerticleLines() const;
	void CreateHorizontalLines() const;


	void Refresh();
	UFUNCTION()
	void RemoveItem(class UItemObject* ItemObject);

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, 
		FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	bool IsRoomAvailableForPayload(class UItemObject* Payload);
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation);
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent);

	void MousePositionInTile(FVector2D MousePosition, bool& bRight, bool& bDown);

	UFUNCTION()
	FEventReply OnMouseButtonDown_Border(FGeometry MyGeometry, const FPointerEvent& MouseEvent);

private:
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UInventoryComponent* InventoryComponent = nullptr;

	UPROPERTY()
	class UCombatComponent* CombatComponent = nullptr;

	UPROPERTY(EditAnywhere, Category = Item)
	class USlateBrushAsset* Brush = nullptr;

	float TileSize;

public:
	UPROPERTY(BlueprintReadWrite)
	mutable TArray<FLineStruct> Lines;

private:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	class UBorder* GridBorder = nullptr;
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	class UCanvasPanel* GridCanvasPanel = nullptr;

	UPROPERTY(EditAnywhere, Category = Item)
	TSubclassOf<class UItemWidget> ItemWidgetClass;

	FVector2D DragTopLeftIndex;

	bool bDrawDropLocation = false;
};
