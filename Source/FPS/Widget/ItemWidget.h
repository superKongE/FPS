#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemWidget.generated.h"

DECLARE_DELEGATE_OneParam(FMultiItemRemoved_OneParam, class UItemObject*);
DECLARE_DELEGATE(FItemRemoved);
/**
 * 
 */
UCLASS()
class FPS_API UItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//virtual bool Initialize() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent);
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent);
	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation);

	void Init(float TileSize_, class UItemObject* ItemObject_, bool bSlotWidget_);

	void Refresh();

	void SetIconImage();
	
private:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	class USizeBox* BackGroundSizeBox = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	class UBorder* BackGroundBorder = nullptr;
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess = "true"))
	class UImage* ItemImage = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UDragDropOperation> DragDropOperationClass;

	UPROPERTY()
	class UItemObject* ItemObject = nullptr;
	UPROPERTY(EditAnywhere, Category = Item)
	float TileSize;
	UPROPERTY(EditAnywhere, Category = Item)
	FVector2D Size;

	bool bSlotWidget = false;
		
public:
	FMultiItemRemoved_OneParam ItemRemoved_OneParam;
	FItemRemoved ItemRemoved;
};
