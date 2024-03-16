#include "FPS/ActorComponent/InventoryComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetArrayLibrary.h"
#include "Engine/SkeletalMeshSocket.h"

#include "FPS/FPSCharacter.h"
#include "FPS/Item/ItemObject.h"
#include "FPS/Widget/InventoryGridWidget.h"
#include "FPS/Widget/InventoryWidget.h"
#include "FPS/Widget/ItemWidget.h"
#include "FPS/Item/Item.h"
#include "FPS/RenderTargetCharacter.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	ItemArray.Init(0, Colums * Rows);
	SlotItemArray.Init(0, 3);
	SlotItemArray[0] = nullptr;
	SlotItemArray[1] = nullptr;
	SlotItemArray[2] = nullptr;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerCharacter)
	{
		OwningPlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
		if (OwningPlayerController)
		{
			if (InventoryGridWidgetClass)
			{
				InventoryGridWidget = CreateWidget<UInventoryGridWidget>(OwningPlayerController, InventoryGridWidgetClass);
				if (InventoryGridWidget)
				{
					InventoryGridWidget->Init(this, TileSize);
				}
			}

			if (InventoryWidgetClass)
			{
				InventoryWidget = CreateWidget<UInventoryWidget>(OwningPlayerController, InventoryWidgetClass);	
				if (InventoryWidget)
				{
					InventoryWidget->Init(this, TileSize);
				}
			}
		}
	}
}
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (IsDirty)
	{
		IsDirty = false;
	
		OnInventoryChanged.Broadcast();
	}
}
void UInventoryComponent::Init(class AFPSCharacter* FPSCharacter)
{
	OwnerCharacter = FPSCharacter;
}


bool UInventoryComponent::SetActiveInventory()
{
	if (InventoryWidget == nullptr || OwningPlayerController == nullptr) return false;

	if (InventoryWidget->IsInViewport())
	{
		InventoryWidget->RemoveFromParent();
		FInputModeGameOnly InputMode;
		OwningPlayerController->SetInputMode(InputMode);
		OwningPlayerController->SetShowMouseCursor(false);
		return false;
	}
	else
	{
		InventoryWidget->AddToViewport(); 
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(InventoryWidget->GetCachedWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false); 
		OwningPlayerController->SetInputMode(InputMode);
		OwningPlayerController->SetShowMouseCursor(true);
		return true;
	}

	return false;
}



// 인벤토리에 아이템 넣을 수 있는지 홧인
bool UInventoryComponent::TryAddItem(UItemObject* ItemObect)
{
	if (ItemObect == nullptr) return false;

	for (int32 i = 0; i < Colums * Rows; i++)
	{
		// 가용 가능한 공간찾기
		if (IsRoomAvailable(ItemObect, i))
		{
			// 인벤토리에 아이템 추가
			AddItem(ItemObect, i);
			return true;
		}
	}

	return false;
}
// 인벤토리에 아이템 넣기
void UInventoryComponent::AddItem(UItemObject* ItemObject, int32 TopLeftIndex)
{
	FTileStruct TileStruct = IndexToTile(TopLeftIndex);
	FIntPoint Dimensions = ItemObject->GetDimensions();

	FIndexArray tempIndexArray;
	for (int32 i = TileStruct.X; i < TileStruct.X + Dimensions.X; i++)
	{
		for (int32 j = TileStruct.Y; j < TileStruct.Y + Dimensions.Y; j++)
		{			
			int32 Index = TileToIndex(FTileStruct(i, j));
			ItemArray[Index] = true;
			tempIndexArray.IndexArray.Emplace(Index);
		}
	}

	ItemObject->SetIsSlotObject(false);
	ItemMaps.Emplace(ItemObject, tempIndexArray);

	IsDirty = true;
}
// 인벤토리에 가용 가능한 공간이 있는지
bool UInventoryComponent::IsRoomAvailable(class UItemObject* ItemObject, int32 TopLeftIndex)
{
	FTileStruct TileStruct = IndexToTile(TopLeftIndex);
	FIntPoint Dimensions = ItemObject->GetDimensions();

	for (int32 i = TileStruct.X; i < TileStruct.X + Dimensions.X; i++)
	{
		for (int32 j = TileStruct.Y; j < TileStruct.Y + Dimensions.Y; j++)
		{
			// 타일 범위에 벗어나는지
			if (IsTileValid(i, j))
			{
				int32 index = TileToIndex(FTileStruct(i, j));
				if (ItemArray[index] == true) return false;
			}
			else
				return false;
		}
	}

	return true;
}


FTileStruct UInventoryComponent::IndexToTile(int32 Index)
{
	FTileStruct TileStruct;
	TileStruct.X = Index % Colums;
	TileStruct.Y = Index / Colums;

	return TileStruct;
}
int32 UInventoryComponent::TileToIndex(FTileStruct TileStruct)
{
	return TileStruct.X + TileStruct.Y * Colums;
}
bool UInventoryComponent::IsTileValid(int32 X, int32 Y)
{
	if (X < 0 || Y < 0 || X >= Rows || Y >= Colums) return false;

	return true;
}


TMap<UItemObject*, FIndexArray>& UInventoryComponent::GetAllItems()
{
	return ItemMaps;
}



void UInventoryComponent::RemoveItem(UItemObject* ItemObject)
{
	if (ItemObject == nullptr) return;

	FIndexArray& IndexArrayStruct = ItemMaps[ItemObject];
	for (int32 index : IndexArrayStruct.IndexArray)
	{
		ItemArray[index] = false;
	}

	ItemMaps.Remove(ItemObject);

	IsDirty = true;
}


void UInventoryComponent::SetItemArray(int32 Index, bool bUse)
{
	ItemArray[Index] = bUse;
}


int32 UInventoryComponent::GetItemObjectTopLeftIndex(UItemObject* ItemObject)
{
	return ItemMaps[ItemObject].IndexArray[0];
}


// 인벤토리에서 아이템 버리는 경우(생성 후 버림)
void UInventoryComponent::DeleteFormInventory(UItemObject* ItemObject)
{
	if (ItemObject == nullptr) return;

	AFPSCharacter* FPSCharacter = Cast<AFPSCharacter>(GetOwner());
	if (FPSCharacter == nullptr) return;

	if (ItemObject->GetItemClass())
	{
		AItem* Item = GetWorld()->SpawnActor<AItem>(ItemObject->GetItemClass(), FPSCharacter->GetWeaponDropPoint(), FPSCharacter->GetActorRotation());
		if (Item)
		{
			Item->Init(ItemObject);
			Item->SetOwner(nullptr);
			Item->DropWeaponState();
			Item->GetItemMesh()->AddImpulse(Item->GetUpVector() * ImpulseZValue);
		}
	}
}
// 슬롯에서 인벤으로 옮기는 경우(생성된것을 파괴만함)
void UInventoryComponent::DeleteFromSlot(UItemObject* ItemObject, bool bDrop)
{
	TArray<AItem*>& WeaponArray = OwnerCharacter->GetWeaponArray();
	int32& CurrentEquippedWeaponIndex = OwnerCharacter->GetCurrentEquippedWeaponIndex();

	bool bEquipNextWeapon = false;
	for (int32 i = 0; i < WeaponArray.Num(); i++)
	{
		if (WeaponArray[i] && ItemObject == WeaponArray[i]->GetItemObject())
		{
			// 장착중인 무기인 경우
			if (WeaponArray[CurrentEquippedWeaponIndex] && WeaponArray[CurrentEquippedWeaponIndex]->GetItemObject() == ItemObject)
			{
				OwnerCharacter->SetbUseLeftHandTransform(false);
				OwnerCharacter->SetIsEquipped(false);
				OwnerCharacter->SetEquippedWeapon(nullptr);

				// 다른 슬롯의 무기 장착하기
				bEquipNextWeapon = true;
			}

			if (bDrop)
			{
				WeaponArray[i]->DropWeaponState();
				//WeaponArray[i]->GetItemMesh()->AddImpulse(WeaponArray[i]->GetUpVector() * 1.f);
				WeaponArray[i]->SetOwner(nullptr);
				WeaponArray[i] = nullptr;
			}
			else
			{
				WeaponArray[i]->Destroy();
				WeaponArray[i] = nullptr;
			}

			OwnerCharacter->GetRenderTargetCharacter()->DropWeapon();
			if (bEquipNextWeapon)
			{
				WeaponArray[i]->GetItemMesh()->AddImpulse(WeaponArray[i]->GetUpVector() * 1.f);
				EquipNextWeapon();
			}

			OwnerCharacter->SetEquippedWeaponCnt(-1);
			break;
		}
	}
}
// 인벤토리 아이템을 슬롯으로 옮기는 과정에서 교환되는 경우
// 슬롯의 무기를 인벤으로 옮기기 성공한 경우
void UInventoryComponent::SlotToInven(int32 Index)
{
	if (OwnerCharacter == nullptr) return;

	TArray<AItem*>& WeaponArray = OwnerCharacter->GetWeaponArray();
	int32& CurrentEquippedWeaponIndex = OwnerCharacter->GetCurrentEquippedWeaponIndex();

	// 현재 장착중인 무기를 인벤에 넣으면
	if (CurrentEquippedWeaponIndex == Index && WeaponArray[CurrentEquippedWeaponIndex])
	{
		OwnerCharacter->SetbUseLeftHandTransform(false);
		OwnerCharacter->SetEquippedWeapon(nullptr);
		WeaponArray[Index]->Destroy();
		WeaponArray[Index] = nullptr;

		OwnerCharacter->GetRenderTargetCharacter()->DropWeapon();
	}
	else if (WeaponArray[CurrentEquippedWeaponIndex])
	{
		WeaponArray[Index]->Destroy();
		WeaponArray[Index] = nullptr;
	};

	OwnerCharacter->SetEquippedWeaponCnt(-1);
}
// 인벤토리 아이템을 슬롯으로 옮기는 과정에서 교환되는 경우
// 슬롯의 무기를 인벤으로 옮기기 실패한 경우
void UInventoryComponent::FailSlotToInven(int32 Index)
{
	if (OwnerCharacter == nullptr) return;

	TArray<AItem*>& WeaponArray = OwnerCharacter->GetWeaponArray();
	int32& CurrentEquippedWeaponIndex = OwnerCharacter->GetCurrentEquippedWeaponIndex();

	// 현재 장착중인 무기를 인벤에 넣기 실패시
	if (CurrentEquippedWeaponIndex == Index && WeaponArray[CurrentEquippedWeaponIndex])
	{
		WeaponArray[Index]->DropWeaponState();
		WeaponArray[Index]->SetOwner(nullptr);
		WeaponArray[Index]->GetItemMesh()->AddImpulse(WeaponArray[Index]->GetUpVector() * 1.f);
		WeaponArray[Index] = nullptr;

		OwnerCharacter->SetEquippedWeapon(nullptr);
		OwnerCharacter->GetRenderTargetCharacter()->DropWeapon();

		// 다른 슬롯의 무기 장착하기
		EquipNextWeapon();
	}
	// 등 뒤에 장착한 무기인 경우
	else if (WeaponArray[Index])
	{
		WeaponArray[Index]->SetOwner(nullptr);
		WeaponArray[Index]->DropWeaponState();
		//WeaponArray[Index]->GetItemMesh()->AddImpulse(WeaponArray[Index]->GetUpVector() * 1.f);
		WeaponArray[Index] = nullptr;
	}

	OwnerCharacter->SetEquippedWeaponCnt(-1);
}
void UInventoryComponent::EquipNextWeapon()
{
	if (OwnerCharacter == nullptr) return;

	int32& CurrentEquippedWeaponIndex = OwnerCharacter->GetCurrentEquippedWeaponIndex();
	TArray<AItem*>& WeaponArray = OwnerCharacter->GetWeaponArray();
	int32 NextIndex = (CurrentEquippedWeaponIndex + 1) % 2;

	// 다음 무기가 있으면
	if (WeaponArray[NextIndex])
	{
		const USkeletalMeshSocket* Hand_r_Socket = OwnerCharacter->GetMesh()->GetSocketByName(FName("hand_r_socket"));
		if (Hand_r_Socket)
		{
			OwnerCharacter->SetbUseLeftHandTransform(true);
			CurrentEquippedWeaponIndex = NextIndex;

			Hand_r_Socket->AttachActor(WeaponArray[NextIndex], OwnerCharacter->GetMesh());
			OwnerCharacter->SetEquippedWeapon(WeaponArray[NextIndex]);

			OwnerCharacter->GetRenderTargetCharacter()->EquipNextWeapon();
		}
	}
}