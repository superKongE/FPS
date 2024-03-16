#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

DECLARE_DELEGATE(FFirstSlotWeaponDrop);
DECLARE_DELEGATE(FSecondSlotWeaponDrop);
DECLARE_DELEGATE(FGrenadeSlotDrop);

UENUM()
enum class ECombatState : uint8 
{ 
	ECS_Idle UMETA(DisplayName = "Idle"), 
	ECS_Fire UMETA(DisplayName = "Fire"),
	ECS_Reload UMETA(DisplayName = "Reload"),
	ECS_ReadyGrenade UMETA(DisplayName = "ReadyGrenade"),
	ECS_FireGrenade UMETA(DisplayName = "FireGrenade"),
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init(class AFPSCharacter* Owner);

	void DrawCrosshair(float DeltaTime);
	void TrachUnderCrossHair();

	void GetItem();
	void EquipWeaponFromSlot(class UItemObject* ItemObject, int32 Index);
	void DropWeapon();
	void EquipNextWeapon();


	void ChangeToFirstWeapon();
	void ChangeToSecondWeapon();
	void AttachWeaponBack(int32 Index);

	class AItem* GetEquippedWeapon();

	void Aim();

	void AimDownSightInfo();

	void ShootPressed();
	void ShootReleased();
	void Shooting();
	UFUNCTION(BlueprintCallable)
	void ShootEnd();

	void Reload();
	UFUNCTION(BlueprintCallable)
	void ReloadEnd();

	void Recoil(class AItem* CurrentEquippedWeapon);
	void ResetRecoil();

	void PlayMontage(ECombatState CombatState);


	void RecoilInterp(float DeltaTime, float InterpSpeed);
	void RecoilInterpToZero(float DeltaTime);


	void RecoilTick(float DeltaTime);
	void RecoilStop();
	void RecoilStart();
	UFUNCTION()
	void RecoilTimerFunction();

	float GetRecoilValue(int32 Cnt);
	
	UFUNCTION(BlueprintCallable)
	void DettachGunMag();
	UFUNCTION(BlueprintCallable)
	void AttachGunMag();

private:
	bool bMovingGunMag = false;

	// 장전을 위해 탄창을 처음 잡았을 때의 탄창의 Transform 정보
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform GunMagTransform;

	// 장전시 손에 부착되어 탄창을 이동시키는데 사용됨
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* HandSceneComponent = nullptr;

private:
	bool IsRecoil = false;
	bool Firing = false;
	bool bRecoilRecovery = false;

	FTimerHandle FireTimer;
	FTimerHandle RecoveryTimer;

	float FireRate = 10.f;

	float RecoveryTime = 0.5f;
	float RecoverySpeed = 10.0f;

	float TargetYaw=0.f;

	// 처음 총을 쏠때 컨트롤러 회전값
	FRotator RecoilStartRot;
	// 현재 프레임의 총기 반동(회전) 값
	FRotator RecoilDeltaRot;
	// 현재 총 회전 값과 사격 시작 시의 총 회전 값의 차이
	FRotator PlayerDeltaRot;
	// 이전 프레임의 반동 값
	FRotator Del;

private:
	UPROPERTY()
	class AFPSCharacter* OwnerCharacter = nullptr;
	UPROPERTY()
	class APlayerController* OwnerPlayerController = nullptr;

	UPROPERTY(EditAnywhere, Category = Anim)
	class UAnimMontage* CombatMontage = nullptr;

	UPROPERTY()
	class AItem* TraceHitItem = nullptr;

	UPROPERTY()
	class AItem* EquippedWeapon = nullptr;


// 카메라 반동 관련
private:
	int32 CurrentMoveRecoilCnt = 0;
	int32 TargetMoveRecoilCnt = 0;
	int32 CurrentStopRecoilCnt = 0;
	int32 TargetStopRecoilCnt = 0;
	int32 RecoilCnt = 0;

	float ShootingTime = 0.f;

	// 현재까지 반동 회복이 진행된 시간
	float CurrentRecoilRecoryTime = 0.f;
	// 반동 UCurve 시작지점
	float RecoilStartPoint = 0.f;

	FTimerHandle RecoilRecoveryEndHandle;


// 사격 관련
private:
	FVector HitLocation;
	bool bAttackButtonPressed = false;

	FTimerHandle ShootTimerHandle;

	// 총을 연속으로 몇발 쐇는지
	int32 ShootCnt = 0;


// 총기 반동 관련
private:
	FTransform RecoilTransform;
	FTransform CurrentRecoilTransform;
	FVector RecoilLocation;

	float TargetRecoilPitch;
	float CurrentRecoilPitch = 0.f;
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float MaxRecoilPitch = -10.f;

	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilStartInterpSpeed = 1.f;
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilEndInterpSpeed = 15.f;

	float CurrentControllerPitch;
	float TargetControllerPitch;
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float MaxTargetControllerPitch;

	float CurrentControllerYaw;
	float TargetControllerYaw;
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float MaxTargetControllerYaw;


// 정조준 관련
private:
	FVector AimSocketLocation;
	FRotator AimSocketRotation;
	FVector AimPointLocation;
	FRotator AimPointRotation;

	FTransform SightTransform;
	FTransform HandTransform;

	FVector WeaponPlacement;

	FVector Target;
	FVector Current;

private:
	bool bIterpAimming = false;
	bool bAimming = false;
	float InitFOV;
	float CurrentFOV = 0.f;
	float TargetFOV;
	UPROPERTY(EditAnywhere, Category = Aimming)
	float AimFOV = 50.f;
	UPROPERTY(EditAnywhere, Category = Aimming)
	float AimInterpSpeed = 5.f;
	UPROPERTY(EditAnywhere, Category = Aimming)
	float NoAimFOV = 100.f;


// TraceUnderCrosshair 관련
private:
	UPROPERTY(EditAnywhere, Category = Trace)
	float TraceDistance = 8000.f;
	UPROPERTY(EditAnywhere, Category = Trace)
	float DetectItemDistance = 150.f;

	UPROPERTY()
	TArray<class AActor*> TraceIgnoreActors;


// CrossHair 퍼짐 관련
private:
	UPROPERTY(EditAnywhere, Category = Crosshair)
	float JogSpread = 10.f;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	float AirSpread = 20.f;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	float CrosshairInterpUpSpeed = 5.f;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	float CrosshairInterpDownSpeed = 10.f;
	float CurrentCrosshairSpread = 0.f;

private:
	ECombatState CurrentCombatState = ECombatState::ECS_Idle;

// 장착 중인 무기를 버리면 
// 델리게이트를 통해 슬롯 위젯에게 알려줘서 
// 슬롯에서 무기를 제거한다
public:
	FFirstSlotWeaponDrop FirstSlotWeaponDrop;
	FSecondSlotWeaponDrop SecondSlotWeaponDrop;
	FGrenadeSlotDrop GrenadeSlotDrop;

public:
	FORCEINLINE FVector GetAimSocketLocation() const { return AimSocketLocation; }
	FORCEINLINE FRotator GetAimSocketRotation() const { return AimSocketRotation; }
	FORCEINLINE FVector GetAimPointLocation() const { return AimPointLocation; }
	FORCEINLINE FRotator GetAimPointRotation() const { return AimPointRotation; }
	FORCEINLINE FTransform GetSightTransform() const { return SightTransform; }
	FORCEINLINE FTransform GetHandTransform() const { return HandTransform; }
	FORCEINLINE void SetCombatState(ECombatState CombatState) { CurrentCombatState = CombatState; }
	FORCEINLINE FTransform GetRecoilTransform() const { return RecoilTransform; }
	FORCEINLINE FTransform GetCurrentRecoilTransform() const { return CurrentRecoilTransform; }
	FORCEINLINE void SetEquippedWeapon(class AItem* Weapon) { EquippedWeapon = Weapon; }
	FORCEINLINE bool GetbMovingGumMag() const { return bMovingGunMag; }
	FORCEINLINE class USceneComponent* GetHandSceneComponent() const { return HandSceneComponent; }
	FORCEINLINE ECombatState GetCurrentCombatState() const { return CurrentCombatState; }
	FORCEINLINE bool GetIsAimming() const { return bAimming; }
};
