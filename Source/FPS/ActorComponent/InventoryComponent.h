#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPS/Struct/LineStruct.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryChanged)

USTRUCT()
struct FIndexArray
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY()
	TArray<int32> IndexArray;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	void Init(class AFPSCharacter* FPSCharacter);
	

	bool SetActiveInventory();

	void AddItem(class UItemObject* ItemObject, int32 TopLeftIndex);
	bool TryAddItem(class UItemObject* ItemObect);
	bool IsRoomAvailable(class UItemObject* ItemObject, int32 TopLeftIndex);
	FTileStruct IndexToTile(int32 Index);
	int32 TileToIndex(FTileStruct TileStruct);

	bool IsTileValid(int32 X, int32 Y);

	int32 GetItemObjectTopLeftIndex(class UItemObject* ItemObject);


	void RemoveItem(class UItemObject* ItemObject);

	void SetItemArray(int32 Index, bool bUse);


	TMap<class UItemObject*, FIndexArray>& GetAllItems();

	/*UFUNCTION()
	void DeleteFromSlotWithDrop(class UItemObject* ItemObject);*/
	void DeleteFormInventory(class UItemObject* ItemObject);
	void DeleteFromSlot(class UItemObject* ItemObject, bool bDrop);
	void EquipNextWeapon();

	void SlotToInven(int32 Index);
	void FailSlotToInven(int32 Index);

private:
	UPROPERTY()
	class AFPSCharacter* OwnerCharacter = nullptr;
	UPROPERTY()
	class APlayerController* OwningPlayerController = nullptr;

	UPROPERTY(EditAnywhere, Category = Widget)
	TSubclassOf<class UInventoryGridWidget> InventoryGridWidgetClass;
	UPROPERTY()
	class UInventoryGridWidget* InventoryGridWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = Widget)
	TSubclassOf<class UInventoryWidget> InventoryWidgetClass;
	UPROPERTY()
	class UInventoryWidget* InventoryWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = Widget)
	TSubclassOf<class UItemWidget> ItemWidgetClass;
	UPROPERTY()
	class UItemWidget* ItemWidget = nullptr;

	UPROPERTY(EditAnywhere, Category = Inventory)
	int32 Colums = 10;
	UPROPERTY(EditAnywhere, Category = Inventory)
	int32 Rows = 10;
	UPROPERTY(EditAnywhere, Category = Inventory)
	float TileSize = 50.f;

	UPROPERTY()
	TMap<class UItemObject*, FIndexArray> ItemMaps;
	TArray<bool> ItemArray;

	bool IsDirty = false;

	bool bDropEventHandled = false;

	UPROPERTY(EditAnywhere, Category = Item)
	float ImpulseZValue = 10.f;

private:
	UPROPERTY()
	TArray<UItemObject*> SlotItemArray;

public:
	FOnInventoryChanged OnInventoryChanged;

public:
	FORCEINLINE int32 GetColums() const { return Colums; }
	FORCEINLINE int32 GetRows() const { return Rows; }
	FORCEINLINE TMap<class UItemObject*, FIndexArray>& GetItemMaps() { return ItemMaps; }
	FORCEINLINE bool GetDropEventHandled() const { return bDropEventHandled; }
	FORCEINLINE void SetDropEventHandled(bool bHandled) { bDropEventHandled = bHandled; }
	FORCEINLINE UItemObject* GetSlotItemObject(int32 Index) const { return SlotItemArray[Index]; }
	FORCEINLINE void SetSlotItemObject(class UItemObject* ItemObject_, int32 Index_) { SlotItemArray[Index_] = ItemObject_; }
};
