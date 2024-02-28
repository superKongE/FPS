#include "FPS/Item/Item.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Curves/CurveVector.h"

#include "FPS/FPSCharacter.h"
#include "FPS/Item/ItemObject.h"
#include "FPS/ActorComponent/InventoryComponent.h"
#include "FPS/Widget/ItemInfoWidget.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	SetRootComponent(SceneComponent);

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetMassScale(FName("NAME_None"), 30.f);
	ItemMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
	ItemMesh->SetupAttachment(SceneComponent);

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetVisibility(false);
	SphereCollision->SetSphereRadius(SphereRadius);
	SphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	SphereCollision->SetupAttachment(ItemMesh);

	ItemInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("ItemInfoWidget"));
	ItemInfoWidgetComponent->SetupAttachment(ItemMesh);
	ItemInfoWidgetComponent->SetVisibility(false);
	if(ItemInfoWidgetClass)
		ItemInfoWidgetComponent->SetWidgetClass(ItemInfoWidgetClass);
}
void AItem::Init(class UItemObject* ItemObject_)
{
	ItemObject = ItemObject_;
}
void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (ItemObjectClass && ItemObject == nullptr)
	{
		ItemObject = NewObject<UItemObject>(this, ItemObjectClass);
	}
	if (ItemInfoWidgetComponent)
	{
		UItemInfoWidget* ItemInfoWidget = Cast<UItemInfoWidget>(ItemInfoWidgetComponent->GetWidget());
		if(ItemInfoWidget)
			ItemInfoWidget->Init(ItemObject->GetItemIcon());

		ItemInfoWidgetComponent->SetVisibility(false);
	}
}
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ItemMesh->IsSimulatingPhysics())
	{
		PrevLocation = CurrentLocation;
		CurrentLocation = GetActorLocation();

		if (PrevLocation == CurrentLocation)
		{
			StopCurrentSimulateTime += DeltaTime;
			if (StopCurrentSimulateTime >= StopSimulateTime)
			{
				StopCurrentSimulateTime = 0.f;

				SphereCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
				ItemMesh->SetSimulatePhysics(false);
				ItemMesh->SetEnableGravity(false);
			}
		}
	}
}


void AItem::SetVisibility(bool bVisible)
{
	if (ItemInfoWidgetComponent && bVisible)
	{
		ItemInfoWidgetComponent->SetVisibility(true);
	}
	else if (ItemInfoWidgetComponent && !bVisible)
	{
		ItemInfoWidgetComponent->SetVisibility(false);
	}
}


void AItem::SetRotation(class AFPSCharacter* FPSCharacter)
{
	if (ItemInfoWidgetComponent)
	{
		FVector Direction = FPSCharacter->GetActorLocation() - GetActorLocation();
		ItemInfoWidgetComponent->SetWorldRotation(Direction.Rotation());
	}
}


void AItem::DropWeaponState()
{
	if (ItemInfoWidgetComponent)
	{
		ItemInfoWidgetComponent->SetVisibility(false);
	}

	CurrentLocation = GetActorLocation();
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld, true);
	DetachFromActor(DetachRules);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ItemMesh->SetSimulatePhysics(true);
	ItemMesh->SetEnableGravity(true);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
void AItem::EquipWeaponState()
{
	if (ItemInfoWidgetComponent)
	{
		ItemInfoWidgetComponent->SetVisibility(false);
	}

	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ItemMesh->SetSimulatePhysics(false);
	ItemMesh->SetEnableGravity(false);
}


bool AItem::Attack(FVector& HitLocation, bool IsRun, bool IsNotSpread)
{
	return true;
}


void AItem::Reload()
{

}
bool AItem::CanReload()
{
	return false;
}

bool AItem::GetCanShooting()
{
	return false;
}

float AItem::GetRecoilRoverTime(int32 Cnt)
{
	if (Cnt <= StartSpreadShootCnt)
		return NeedForRecoilTime;

	return NeedForRecoilfullTime;
}

FVector AItem::GetRecoilCurveValue(float Time)
{
	if (RecoilCurve)
	{
		return RecoilCurve->GetVectorValue(Time);
	}

	return FVector::ZeroVector;
}



FVector AItem::GetUpVector()
{
	return UpVector;
}