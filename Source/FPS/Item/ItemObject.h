#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ItemObject.generated.h"


UCLASS(Blueprintable, BlueprintType)
class FPS_API UItemObject : public UObject
{
	GENERATED_BODY()

public:
	UItemObject();

	class UMaterialInstance* GetIcon();
	FIntPoint GetDimensions();
	FIntPoint GetUnRotateDimensions();

	void Rotate();
	
private:
	UPROPERTY(EditAnywhere, Category = Item)
	FIntPoint Dimensions;
	UPROPERTY(EditAnywhere, Category = Item)
	int32 IconWidth = 4;
	UPROPERTY(EditAnywhere, Category = Item)
	int32 IconHeight = 2;

	UPROPERTY(EditAnywhere, Category = Item)
	class UMaterialInstance* Icon = nullptr;
	UPROPERTY(EditAnywhere, Category = Item)
	class UMaterialInstance* IconRotated = nullptr;
	UPROPERTY(EditAnywhere, Category = Item)
	TSubclassOf<class AItem> ItemClass;

	UPROPERTY(EditAnywhere, Category = Item)
	FName ItemName;

	bool bRotated = false;
	bool bSlotObject = false;

public:
	FORCEINLINE TSubclassOf<class AItem> GetItemClass() const { return ItemClass; }
	FORCEINLINE bool GetIsRotated() const { return bRotated; }
	FORCEINLINE FName GetItemName() const { return ItemName; }
	FORCEINLINE UMaterialInstance* GetItemIcon() const { return Icon; }
	FORCEINLINE bool GetIsSlotObject() const { return bSlotObject; }
	FORCEINLINE void SetIsSlotObject(bool bSlot) { bSlotObject = bSlot; }
};
