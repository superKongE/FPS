#include "FPS/RenderTargetCharacter.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "FPS/SceneCapture/CharacterCapture2D.h"
#include "FPS/FPSCharacter.h"
#include "FPS/Item/Item.h"

ARenderTargetCharacter::ARenderTargetCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneCaptrueSpawnLocation = CreateDefaultSubobject<UArrowComponent>(TEXT("SceneCaptrueSpawnLocation"));
	SceneCaptrueSpawnLocation->SetupAttachment(RootComponent);
	SceneCaptrueSpawnLocation->SetVisibility(false);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
}
void ARenderTargetCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (SceneCapture2DClass)
	{
		SceneCapture2D = GetWorld()->SpawnActor<ACharacterCapture2D>(SceneCapture2DClass, SceneCaptrueSpawnLocation->GetComponentLocation(), SceneCaptrueSpawnLocation->GetComponentRotation());
		ShowOnlyActors.Emplace(this);
		SceneCapture2D->GetCaptureComponent2D()->ShowOnlyActors = ShowOnlyActors;
	}
}
void ARenderTargetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bEquipped && EquippedWeapon)
	{
		const USkeletalMeshSocket* Hand_l_Socket = EquippedWeapon->GetItemMesh()->GetSocketByName(FName("LeftHand"));
		if(Hand_l_Socket)
			LeftHandTransform = Hand_l_Socket->GetSocketLocation(EquippedWeapon->GetItemMesh());
	}
}
void ARenderTargetCharacter::Init(AFPSCharacter* FPSCharacter)
{
	if (FPSCharacter == nullptr) return;
	
	if (SceneCapture2D)
	{
		SceneCapture2D->Init(this);
	}

	GetMesh()->SetAnimClass(FPSCharacter->GetMesh()->GetAnimClass());
	GetMesh()->SetSkeletalMesh(FPSCharacter->GetMesh()->SkeletalMesh);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -GetCapsuleComponent()->GetScaledCapsuleHalfHeight()));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
}


void ARenderTargetCharacter::EquipWeapon(TSubclassOf<class AItem> ItemClass)
{
	if (ItemClass == nullptr) return;

	const USkeletalMeshSocket* Hand_r_Socket = GetMesh()->GetSocketByName(FName("hand_r_socket"));
	FVector Location = Hand_r_Socket->GetSocketLocation(GetMesh());

	FRotator Rotation = Hand_r_Socket->GetSocketTransform(GetMesh()).Rotator();
	FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	AItem* Item = GetWorld()->SpawnActor<AItem>(ItemClass, Location, Hand_r_Socket->GetSocketTransform(GetMesh()).Rotator());
	if (Item)
	{
		// 이미 장착중인 무기가 있으면 파괴
		if (EquippedWeapon)
		{
			ShowOnlyActors.Remove(EquippedWeapon);

			EquippedWeapon->Destroy();
		}

		ShowOnlyActors.Emplace(Item);
		SceneCapture2D->GetCaptureComponent2D()->ShowOnlyActors = ShowOnlyActors;

		bEquipped = true;
		EquippedWeapon = Item;
		Item->SetOwner(this);
		Item->EquipWeaponState();
		Hand_r_Socket->AttachActor(Item, GetMesh());
	}
}
void ARenderTargetCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		bEquipped = false;
		ShowOnlyActors.Remove(EquippedWeapon);
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;

		SceneCapture2D->GetCaptureComponent2D()->ShowOnlyActors = ShowOnlyActors;
	}
}
void ARenderTargetCharacter::AttachWeaponBack(TSubclassOf<class AItem> ItemClass)
{
	const USkeletalMeshSocket* WeaponSocket = GetMesh()->GetSocketByName(FName("FirstWeaponPoint"));
	if (WeaponSocket == nullptr || ItemClass == nullptr) return;

	AItem* Item = GetWorld()->SpawnActor<AItem>(ItemClass, WeaponSocket->GetSocketLocation(GetMesh()), WeaponSocket->GetSocketTransform(GetMesh()).Rotator());
	if (Item)
	{
		if (WeaponSocket)
		{
			if (EquippedBackWeapon)
			{
				ShowOnlyActors.Remove(EquippedBackWeapon);

				EquippedBackWeapon->Destroy();
			}

			ShowOnlyActors.Emplace(Item);
			SceneCapture2D->GetCaptureComponent2D()->ShowOnlyActors = ShowOnlyActors;

			EquippedBackWeapon = Item;

			WeaponSocket->AttachActor(Item, GetMesh());
		}
	}
}
void ARenderTargetCharacter::EquipNextWeapon()
{
	const USkeletalMeshSocket* Hand_r_Socket = GetMesh()->GetSocketByName(FName("hand_r_socket"));
	if (Hand_r_Socket)
	{
		if (EquippedWeapon)
		{
			ShowOnlyActors.Remove(EquippedWeapon);
			SceneCapture2D->GetCaptureComponent2D()->ShowOnlyActors = ShowOnlyActors;

			EquippedWeapon->Destroy();
		}

		if (EquippedBackWeapon)
		{
			EquippedWeapon = EquippedBackWeapon;
			EquippedBackWeapon = nullptr;

			Hand_r_Socket->AttachActor(EquippedWeapon, GetMesh());
		}
	}
}


void ARenderTargetCharacter::AddRotationYaw(float Yaw)
{
	SetActorRotation(GetActorRotation() + FRotator(0.f, Yaw, 0.f));
}