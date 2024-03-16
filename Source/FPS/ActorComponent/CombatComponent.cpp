#include "FPS/ActorComponent/CombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/ArrowComponent.h"

#include "FPS/FPSCharacter.h"
#include "FPS/Item/ItemObject.h"
#include "FPS/Item/Item.h"
#include "FPS/ActorComponent/InventoryComponent.h"
#include "FPS/RenderTargetCharacter.h"
#include "FPS/HUD/FPSHUD.h"
#include "FPS/Item/AK47.h"
#include "FPS/Item/Rifle.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	HandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComponent"));
}
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerCharacter)
	{
		CurrentFOV = InitFOV = OwnerCharacter->GetCurrentCamera()->FieldOfView;
		OwnerPlayerController = OwnerCharacter->GetFPSController();
	}
}
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	TrachUnderCrossHair();
	DrawCrosshair(DeltaTime);

	if (TraceHitItem)
	{
		TraceHitItem->SetRotation(OwnerCharacter);
	}

	if (bIterpAimming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, TargetFOV, DeltaTime, AimInterpSpeed);
		OwnerCharacter->GetCurrentCamera()->SetFieldOfView(CurrentFOV);
	}

	if (bAttackButtonPressed)
	{
		RecoilInterp(DeltaTime, RecoilStartInterpSpeed);
		RecoilInterpToZero(DeltaTime);
	}
	else
	{
		RecoilInterpToZero(DeltaTime);
		RecoilInterp(DeltaTime, RecoilEndInterpSpeed);
	}

	RecoilTick(DeltaTime);
}
void UCombatComponent::Init(class AFPSCharacter* Owner)
{
	OwnerCharacter = Owner;

	TraceIgnoreActors.Emplace(OwnerCharacter);
}
void UCombatComponent::TrachUnderCrossHair()
{
	if (OwnerCharacter == nullptr) return;

	APlayerController* PlayerController = Cast<APlayerController>(OwnerCharacter->GetController());
	if (PlayerController == nullptr) return;

	FVector2D Viewport;
	GEngine->GameViewport->GetViewportSize(Viewport);

	FVector2D CrossHairPosition = Viewport / 2.f;
	FVector WorldPosition;
	FVector WorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(PlayerController, CrossHairPosition, WorldPosition, WorldDirection);

	// 3인칭의 경우 카메라가 캐릭터 뒤에 있으므로 카메라와 캐릭터의 거리만큼 앞으로 이동
	float DistanceToCharacter = (OwnerCharacter->GetActorLocation() - WorldPosition).Size();
	WorldPosition += WorldDirection * (DistanceToCharacter + 10.f);

	FHitResult HitResult;
	FCollisionQueryParams FQP;
	FQP.AddIgnoredActors(TraceIgnoreActors);
	GetWorld()->LineTraceSingleByChannel(HitResult, WorldPosition, WorldPosition + WorldDirection * TraceDistance, ECollisionChannel::ECC_Visibility, FQP);
//	DrawDebugLine(GetWorld(), WorldPosition, WorldPosition + WorldDirection * TraceDistance, FColor::Red, true);

	if (HitResult.bBlockingHit)
	{
		AItem* Item = Cast<AItem>(HitResult.GetActor());
		float DistanceToItem = (HitResult.ImpactPoint - OwnerCharacter->GetActorLocation()).Size();
		HitLocation = HitResult.ImpactPoint;
		if (Item && DistanceToItem <= DetectItemDistance)
		{
			if(TraceHitItem && TraceHitItem != Item)
				TraceHitItem->SetVisibility(false);
			TraceHitItem = Item;
			TraceHitItem->SetVisibility(true);
		}
		else
		{
			if (TraceHitItem)
				TraceHitItem->SetVisibility(false);
			TraceHitItem = nullptr;
		}
	}
	else 
	{
		HitLocation = WorldPosition + WorldDirection * TraceDistance;
		if (TraceHitItem)
			TraceHitItem->SetVisibility(false);
		TraceHitItem = nullptr;
	}
}
void UCombatComponent::DrawCrosshair(float DeltaTime)
{
	if (OwnerCharacter == nullptr) return;

	FCrossHairInfo* CrossHairInfo = OwnerCharacter->GetFPSHUD()->GetCrossHairInfo();
	if (CrossHairInfo == nullptr) return;

	if (bAimming && OwnerCharacter->GetIsFirstPersonCamera())
	{
		CrossHairInfo->bDraw = false;
	}
	else
	{
		CrossHairInfo->bDraw = true;

		float Speed = OwnerCharacter->GetVelocity().Size();
		bool IsInAir = OwnerCharacter->GetCharacterMovement()->IsFalling();

		float TargetCrossSpread = 0.f;
		if (Speed > OwnerCharacter->GetWalkSpeed())
			TargetCrossSpread += JogSpread;
		if (IsInAir)
			TargetCrossSpread += AirSpread;

		if (CurrentCrosshairSpread < TargetCrossSpread)
			CurrentCrosshairSpread = FMath::FInterpTo(CurrentCrosshairSpread, TargetCrossSpread, DeltaTime, CrosshairInterpUpSpeed);
		else
			CurrentCrosshairSpread = FMath::FInterpTo(CurrentCrosshairSpread, TargetCrossSpread, DeltaTime, CrosshairInterpDownSpeed);

		CrossHairInfo->CrosshairSpread = CurrentCrosshairSpread;
	}
}


void UCombatComponent::GetItem()
{
	if (TraceHitItem)
	{
		UItemObject* ItemObject = TraceHitItem->GetItemObject();
		UInventoryComponent* InventoryComponent = OwnerCharacter->GetInventoryComponent();
		if (InventoryComponent && ItemObject)
		{
			if (InventoryComponent->TryAddItem(ItemObject))
			{
				TraceHitItem->Destroy();
				TraceHitItem = nullptr;
			}
		}
	}
}


// 인벤에서 슬롯으로 무기를 옮기면 장착하기 위해 호출됨                           Index : 어느 슬롯인지
void UCombatComponent::EquipWeaponFromSlot(UItemObject* ItemObject, int32 Index)
{
	if (OwnerCharacter == nullptr || ItemObject == nullptr || CurrentCombatState != ECombatState::ECS_Idle) return;

	const USkeletalMeshSocket* Hand_r_Socket = OwnerCharacter->GetMesh()->GetSocketByName(FName("hand_r_socket"));
	if (Hand_r_Socket == nullptr) return;

	FRotator Rotation = Hand_r_Socket->GetSocketTransform(OwnerCharacter->GetMesh()).Rotator();
	FRotator YawRotation(0, Rotation.Yaw, 0);
	FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	TArray<AItem*>& WeaponArray = OwnerCharacter->GetWeaponArray();
	int32& CurrentEquippedWeaponIndex = OwnerCharacter->GetCurrentEquippedWeaponIndex();

	AItem* Item = GetWorld()->SpawnActor<AItem>(ItemObject->GetItemClass(), OwnerCharacter->GetActorLocation(), FRotator::ZeroRotator);
	if (Item)
	{
		Item->Init(ItemObject);
		Item->SetOwner(OwnerCharacter);
		Item->EquipWeaponState();

		OwnerCharacter->SetIsEquipped(true);
		OwnerCharacter->SetbUseLeftHandTransform(true);

		// 처음 장착하거나 장착중인 무기와 인벤토리 무기를 바꾸면
		if (CurrentEquippedWeaponIndex == Index || OwnerCharacter->GetEquippedWeaponCnt() == 0)
		{
			CurrentEquippedWeaponIndex = Index;
			if (WeaponArray[Index])
				WeaponArray[Index]->Destroy();

			WeaponArray[Index] = Item;
			EquippedWeapon = Item;
			EquippedWeapon->SetOwner(OwnerCharacter);

			FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, false);
			Hand_r_Socket->AttachActor(Item, OwnerCharacter->GetMesh());

			OwnerCharacter->GetRenderTargetCharacter()->EquipWeapon(ItemObject->GetItemClass());
		}
		// 처음 장착이 아니거나 등 두의 무기와 인벤토리 무기를 바꾸면
		else
		{
			if(WeaponArray[Index])
				WeaponArray[Index]->Destroy();
			WeaponArray[Index] = Item;
			WeaponArray[Index]->SetOwner(OwnerCharacter);

			AttachWeaponBack((CurrentEquippedWeaponIndex + 1) % 2);

			OwnerCharacter->GetRenderTargetCharacter()->AttachWeaponBack(ItemObject->GetItemClass());
		}
	}
	OwnerCharacter->SetEquippedWeaponCnt(1);
}
// g키를 누르면 들고있는 무기를 버림
void UCombatComponent::DropWeapon()
{
	if (OwnerCharacter == nullptr || CurrentCombatState != ECombatState::ECS_Idle) return;

	int32& CurrentEquippedWeaponIndex = OwnerCharacter->GetCurrentEquippedWeaponIndex();

	const USkeletalMeshSocket* Hand_r_Socket = OwnerCharacter->GetMesh()->GetSocketByName(FName("hand_r_socket"));
	if (Hand_r_Socket && EquippedWeapon)
	{
		OwnerCharacter->SetbUseLeftHandTransform(false);
		TraceHitItem = nullptr;
		OwnerCharacter->SetEquippedWeaponCnt(-1);
		OwnerCharacter->SetIsEquipped(false);

		EquippedWeapon->SetOwner(nullptr);
		EquippedWeapon->DropWeaponState();
		EquippedWeapon->GetSphereCollision()->AddImpulse(EquippedWeapon->GetUpVector() * 1.f);
		EquippedWeapon = nullptr;

		OwnerCharacter->GetRenderTargetCharacter()->DropWeapon();

		// 슬롯에서 무기 제거
		if (CurrentEquippedWeaponIndex == 0)
			FirstSlotWeaponDrop.Execute();
		else
			SecondSlotWeaponDrop.Execute();

		// 다른 슬롯의 무기 장착하기
		EquipNextWeapon();
		OwnerCharacter->GetRenderTargetCharacter()->EquipNextWeapon();
	}
}
// 장착중인 무기를 버렸을때 다음 무기를 자동으로 장착하기
void UCombatComponent::EquipNextWeapon()
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
			EquippedWeapon = WeaponArray[NextIndex];
			
			Hand_r_Socket->AttachActor(WeaponArray[NextIndex], OwnerCharacter->GetMesh());
		}
	}
}


// 1번 무기로 전환
void UCombatComponent::ChangeToFirstWeapon()
{
	if (OwnerCharacter != nullptr && CurrentCombatState == ECombatState::ECS_Idle)
	{
		TArray<AItem*>& WeaponArray = OwnerCharacter->GetWeaponArray();
		int32& CurrentEquippedWeaponIndex = OwnerCharacter->GetCurrentEquippedWeaponIndex();

		// 장착한 무기가 없거나 무기 1개만 착용한 경우
		if (OwnerCharacter->GetEquippedWeaponCnt() == 0 || WeaponArray[0] == nullptr || WeaponArray[1] == nullptr) return;

		AttachWeaponBack(1);

		CurrentEquippedWeaponIndex = 0;
		const USkeletalMeshSocket* Hand_r_Socket = OwnerCharacter->GetMesh()->GetSocketByName(FName("hand_r_socket"));
		if (Hand_r_Socket)
		{
			Hand_r_Socket->AttachActor(WeaponArray[0], OwnerCharacter->GetMesh());
			EquippedWeapon = WeaponArray[0];

			OwnerCharacter->GetRenderTargetCharacter()->EquipWeapon(WeaponArray[0]->GetItemObject()->GetItemClass());
		}
	}
}
// 2번 무기로 전환
void UCombatComponent::ChangeToSecondWeapon()
{
	if (OwnerCharacter != nullptr && CurrentCombatState == ECombatState::ECS_Idle)
	{
		TArray<AItem*>& WeaponArray = OwnerCharacter->GetWeaponArray();
		int32& CurrentEquippedWeaponIndex = OwnerCharacter->GetCurrentEquippedWeaponIndex();

		// 장착한 무기가 없거나 무기 1개만 착용한 경우
		if (OwnerCharacter->GetEquippedWeaponCnt() == 1 || WeaponArray[0] == nullptr || WeaponArray[1] == nullptr) return;

		AttachWeaponBack(0);

		CurrentEquippedWeaponIndex = 1;
		const USkeletalMeshSocket* Hand_r_Socket = OwnerCharacter->GetMesh()->GetSocketByName(FName("hand_r_socket"));
		if (Hand_r_Socket)
		{
			Hand_r_Socket->AttachActor(WeaponArray[1], OwnerCharacter->GetMesh());
			EquippedWeapon = WeaponArray[1];

			OwnerCharacter->GetRenderTargetCharacter()->EquipWeapon(WeaponArray[1]->GetItemObject()->GetItemClass());
		}
	}
}
// 무기를 등 뒤에 달기
void UCombatComponent::AttachWeaponBack(int32 Index)
{
	if (OwnerCharacter == nullptr) return;

	TArray<AItem*>& WeaponArray = OwnerCharacter->GetWeaponArray();
	int32& CurrentEquippedWeaponIndex = OwnerCharacter->GetCurrentEquippedWeaponIndex();

	if (Index < 0 || Index > OwnerCharacter->GetEquippedWeaponCnt()) return;

	const USkeletalMeshSocket* WeaponSocket = OwnerCharacter->GetMesh()->GetSocketByName(OwnerCharacter->GetWeaponSocketNameArray()[Index]);
	if (WeaponSocket)
	{
		WeaponSocket->AttachActor(WeaponArray[Index], OwnerCharacter->GetMesh());

		OwnerCharacter->GetRenderTargetCharacter()->AttachWeaponBack(WeaponArray[Index]->GetItemObject()->GetItemClass());
	}
}


void UCombatComponent::Aim()
{
	if (OwnerCharacter == nullptr) return;

	TArray<AItem*>& WeaponArray = OwnerCharacter->GetWeaponArray();
	int32& CurrentEquippedWeaponIndex = OwnerCharacter->GetCurrentEquippedWeaponIndex();
	if (CurrentEquippedWeaponIndex == -1)return;

	bAimming = !bAimming;
	if (bAimming)
	{
		AimDownSightInfo();
		TargetFOV = AimFOV;
	}
	else
	{
		TargetFOV = NoAimFOV;
	}

	bIterpAimming = true;
}


// 정조준
void UCombatComponent::AimDownSightInfo()
{
	if (OwnerCharacter == nullptr) return;

	TArray<AItem*>& WeaponArray = OwnerCharacter->GetWeaponArray();
	int32& CurrentEquippedWeaponIndex = OwnerCharacter->GetCurrentEquippedWeaponIndex();
	if (CurrentEquippedWeaponIndex == -1) return;

	// 카메라에서부터 시야 방향으로 20만큼 떨어진곳
	FTransform CameraTransform = OwnerCharacter->GetFirstPersonCamera()->GetComponentTransform();
	CameraTransform.SetLocation(CameraTransform.GetLocation() + CameraTransform.GetRotation().GetForwardVector() * 20.f);

	// 총 가늠쇠 
	FTransform AimSocketTransform = WeaponArray[CurrentEquippedWeaponIndex]->GetItemMesh()->GetSocketTransform(FName("AimSocket"));
	// 총을 쥔 손
	FTransform RightHandTransform = OwnerCharacter->GetMesh()->GetSocketTransform(FName("hand_r"));

	HandTransform = UKismetMathLibrary::MakeRelativeTransform(CameraTransform, AimSocketTransform);
}


void UCombatComponent::ShootPressed()
{
	if (OwnerCharacter == nullptr || EquippedWeapon == nullptr || bAttackButtonPressed || OwnerCharacter->GetIsInventoryOpen() || CurrentCombatState != ECombatState::ECS_Idle) return;

	bAttackButtonPressed = true;

	if (EquippedWeapon->GetCanShooting())
	{
		bool IsRecoveryEnd = true;

		// 반동 회복 시간이 안끝났으면
		if (GetWorld()->GetTimerManager().TimerExists(RecoilRecoveryEndHandle))
		{
			GetWorld()->GetTimerManager().ClearTimer(RecoilRecoveryEndHandle);
			RecoilStartPoint = 0.6f;
			IsRecoveryEnd = false;
		}
		else
		{
			RecoilStartPoint = 0.f;
			IsRecoveryEnd = true;
		}

		bool IsRun = OwnerCharacter->GetIsJogging();
		bool IsNotSpread = ShootCnt <= EquippedWeapon->GetStartSpreadShootCnt() && IsRecoveryEnd;
		EquippedWeapon->Attack(HitLocation, IsRun, IsNotSpread);

		float ShootingDelay = EquippedWeapon->GetShootingDelay();

		ShootCnt++;
		Recoil(EquippedWeapon);
		RecoilStart();

		CurrentCombatState = ECombatState::ECS_Fire;

		GetWorld()->GetTimerManager().SetTimer(ShootTimerHandle, this, &UCombatComponent::Shooting, ShootingDelay, true, ShootingDelay);
	}
}
void UCombatComponent::ShootReleased()
{
	if (!bAttackButtonPressed) return;

	bAttackButtonPressed = false;
	CurrentCombatState = ECombatState::ECS_Idle;

	ResetRecoil();
	RecoilStop();

	GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
}
void UCombatComponent::Shooting()
{
	if (!bAttackButtonPressed || OwnerCharacter == nullptr) return;

	if (EquippedWeapon == nullptr)
	{
		CurrentCombatState = ECombatState::ECS_Idle;
		ResetRecoil();
		RecoilStop();
		GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
		return;
	}

	// 탄창을 다 쓰는 등 발사 실패시
	if (!EquippedWeapon->GetCanShooting())
	{
		CurrentCombatState = ECombatState::ECS_Idle;
		ResetRecoil();
		RecoilStop();
		GetWorld()->GetTimerManager().ClearTimer(ShootTimerHandle);
		return;
	}
	else
	{
		ShootCnt++;

		bool IsRun = OwnerCharacter->GetIsJogging();
		bool IsNotSpread = ShootCnt <= EquippedWeapon->GetStartSpreadShootCnt() && ShootingTime <= EquippedWeapon->GetNeedForRecoilTime();
		EquippedWeapon->Attack(HitLocation, IsRun, IsNotSpread);

		TargetYaw = GetRecoilValue(ShootCnt);

		Recoil(EquippedWeapon);
	}
}
void UCombatComponent::ShootEnd()
{
	CurrentCombatState = ECombatState::ECS_Idle;
}


void UCombatComponent::Reload()
{
	if (OwnerCharacter == nullptr) return;

	if (EquippedWeapon && EquippedWeapon->CanReload() && CurrentCombatState == ECombatState::ECS_Idle)
	{
		bAimming = false;
		CurrentCombatState = ECombatState::ECS_Reload;

		OwnerCharacter->SetbUseLeftHandTransform(false);

		PlayMontage(ECombatState::ECS_Reload);
	}
}
void UCombatComponent::ReloadEnd()
{
	if (EquippedWeapon == nullptr) return;

	EquippedWeapon->Reload();

	CurrentCombatState = ECombatState::ECS_Idle;

	OwnerCharacter->SetbUseLeftHandTransform(true);
}



void UCombatComponent::PlayMontage(ECombatState CombatState)
{
	if (CombatMontage == nullptr) return;

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		FName SectionName;

		switch (CombatState)
		{
		case ECombatState::ECS_Reload :
			SectionName = FName("Reload");
			break;
		}

		AnimInstance->Montage_Play(CombatMontage);
		AnimInstance->Montage_JumpToSection(SectionName);
	}
}


// 특정 발수 이상 쏘면 좌우 반동이 반복되도록 함
float UCombatComponent::GetRecoilValue(int32 Cnt)
{
	if (Cnt > 7)
	{
		if (CurrentMoveRecoilCnt == TargetMoveRecoilCnt && CurrentStopRecoilCnt == TargetStopRecoilCnt)
		{
			RecoilCnt++;
			CurrentMoveRecoilCnt = 0;
			CurrentStopRecoilCnt = 0;
			TargetMoveRecoilCnt = FMath::RandRange(4, 7);
			TargetStopRecoilCnt = FMath::RandRange(1, 5);
		}

		if (CurrentMoveRecoilCnt != TargetMoveRecoilCnt)
		{
			CurrentMoveRecoilCnt++;

			if (RecoilCnt % 2 == 1)
				return -10.f;
			else
				return 10.f;
		}
		else if (CurrentStopRecoilCnt != TargetStopRecoilCnt)
		{
			CurrentStopRecoilCnt++;
		}
	}

	return 0.f;
}
// 총기 반동을 위해 총을 얼마만큼 이동시킬지 (총 떨림)
void UCombatComponent::Recoil(AItem* CurrentEquippedWeapon)
{
	if (OwnerCharacter == nullptr) return;
	
	float Multipler = CurrentEquippedWeapon->GetRecoilScale();
	float Y = FMath::RandRange(-1.1f, -2.1f);

	RecoilLocation = FVector(0.f, Multipler * Y, 0.f);
	RecoilTransform.SetLocation(RecoilLocation);

	TargetRecoilPitch = MaxRecoilPitch;
}
// 카메라 반동 관련 변수 초기화
void UCombatComponent::ResetRecoil()
{
	CurrentMoveRecoilCnt = 0;
	TargetMoveRecoilCnt = 0;
	CurrentStopRecoilCnt = 0;
	TargetStopRecoilCnt = 0;
	RecoilCnt = 0;
}
// 총기 반동 시작 (손의 회전값을 조절해 총이 들리게끔)
void UCombatComponent::RecoilInterp(float DeltaTime, float InterpSpeed)
{
	CurrentRecoilPitch = FMath::FInterpTo(CurrentRecoilPitch, TargetRecoilPitch, DeltaTime, InterpSpeed);
	RecoilTransform.SetRotation(FQuat(FRotator(0.f, 0.f, CurrentRecoilPitch)));

	CurrentRecoilTransform = UKismetMathLibrary::TInterpTo(CurrentRecoilTransform, RecoilTransform, DeltaTime, 25.f);
}
// 반동 돌아오기
void UCombatComponent::RecoilInterpToZero(float DeltaTime)
{
	FTransform ZeroTransform;
	ZeroTransform.SetLocation(FVector(0.f, 0.f, 0.f));
	ZeroTransform.SetRotation(FQuat(FRotator(0.f, 0.f, 0.f)));

	TargetRecoilPitch = FMath::FInterpTo(TargetRecoilPitch, 0.f, DeltaTime, RecoilEndInterpSpeed);
	RecoilTransform.SetRotation(FQuat(FRotator(0.f, 0.f, TargetRecoilPitch)));

	RecoilTransform = UKismetMathLibrary::TInterpTo(RecoilTransform, ZeroTransform, DeltaTime, 15.f);
}


AItem* UCombatComponent::GetEquippedWeapon()
{
	TArray<AItem*>& WeaponArray = OwnerCharacter->GetWeaponArray();
	int32& CurrentEquippedWeaponIndex = OwnerCharacter->GetCurrentEquippedWeaponIndex();
	if (CurrentEquippedWeaponIndex == -1) return nullptr;

	return WeaponArray[CurrentEquippedWeaponIndex];
}



// 틱에서 호출됨 (총기 반동을 위한 컨트롤러 회전을 담당)
void UCombatComponent::RecoilTick(float DeltaTime)
{
	OwnerPlayerController = OwnerPlayerController == nullptr ?
		Cast<APlayerController>(OwnerCharacter->GetController()) : OwnerPlayerController;
	if (OwnerPlayerController == nullptr || EquippedWeapon == nullptr) return;

	float recoiltime;
	FVector RecoilVec;

	if (IsRecoil)
	{	
		// recoiltime : 총을 몇초 동안 쐇는지
		recoiltime = GetWorld()->GetTimerManager().GetTimerElapsed(FireTimer) + RecoilStartPoint;
		// 커브에서 해당 시간의 값을 가져오기
		RecoilVec = EquippedWeapon->GetRecoilCurveValue(recoiltime);

		// 반동 값 설정
		Del.Roll = 0;
		Del.Pitch = RecoilVec.Y;
		// 총을 어느정도 쏘면 컨트롤러는 더이상 위로 회전하지 않고 좌우로 랜덤하게 회전​
		if (RecoilVec.Z == 0.f)
			Del.Yaw = FMath::FInterpTo(Del.Yaw, TargetYaw, DeltaTime, 1.f);
		else
			Del.Yaw = RecoilVec.Z;

		PlayerDeltaRot = OwnerPlayerController->GetControlRotation() - RecoilStartRot - RecoilDeltaRot;
		OwnerPlayerController->SetControlRotation(RecoilStartRot + PlayerDeltaRot + Del); 
		RecoilDeltaRot = Del;

		if (!Firing && recoiltime > 0.f)
		{
			GetWorld()->GetTimerManager().ClearTimer(FireTimer);
			IsRecoil = false;

			bRecoilRecovery = true;
		}
	}
	// 반동 회복
	else if (bRecoilRecovery)
	{
		FRotator tmprot = OwnerPlayerController->GetControlRotation();

		OwnerPlayerController->SetControlRotation(UKismetMathLibrary::RInterpTo(tmprot, tmprot - RecoilDeltaRot, DeltaTime, 10.0f));

		//                                        (현재 프레임과 이전 프레임간의 컨트롤러 회전 차이)
		RecoilDeltaRot = RecoilDeltaRot + (OwnerPlayerController->GetControlRotation() - tmprot);
	}
}
// 반동을 위한(컨트롤러 회전) 회전 초기 값 세팅
void UCombatComponent::RecoilStart()
{
	OwnerPlayerController = OwnerPlayerController == nullptr ? Cast<APlayerController>(OwnerCharacter->GetController()) : OwnerPlayerController;
	if (OwnerPlayerController == nullptr) return;

	PlayerDeltaRot = FRotator(0.0f, 0.0f, 0.0f);
	RecoilDeltaRot = FRotator(0.0f, 0.0f, 0.0f);
	Del = FRotator(0.0f, 0.0f, 0.0f);
	TargetYaw = 0.f;
	RecoilStartRot = OwnerPlayerController->GetControlRotation();

	Firing = true;

	// FireTimer 타이머를 이용해 총을 몇초동안 쐇는지 알아냄
	GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &UCombatComponent::RecoilTimerFunction, 100.f, false);

	IsRecoil = true;
	bRecoilRecovery = false;
}
void UCombatComponent::RecoilStop()
{
	Firing = false;

	if (EquippedWeapon)
		ShootCnt = 0;
}
void UCombatComponent::RecoilTimerFunction()
{
	GetWorld()->GetTimerManager().PauseTimer(FireTimer);
}


// 애니메이션의 노티파이에 의해 호출됨
// 탄창을 총에 때기
void UCombatComponent::DettachGunMag()
{
	if (EquippedWeapon == nullptr || OwnerCharacter == nullptr || HandSceneComponent == nullptr) return;

	int32 GunMagIndex = EquippedWeapon->GetItemMesh()->GetBoneIndex(EquippedWeapon->GetGunMagName());
	GunMagTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(GunMagIndex);

	FAttachmentTransformRules AttachmentRule(EAttachmentRule::KeepRelative, true);
	HandSceneComponent->AttachToComponent(OwnerCharacter->GetMesh(), AttachmentRule, FName("hand_l"));
	HandSceneComponent->SetWorldTransform(GunMagTransform);

	bMovingGunMag = true;

}
// 탄창을 총에 붙이기
void UCombatComponent::AttachGunMag()
{
	bMovingGunMag = false;
}