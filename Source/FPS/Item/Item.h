#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class FPS_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	void Init(class UItemObject* ItemObject_);

	void SetRotation(class AFPSCharacter* FPSCharacter);

	void SetVisibility(bool bVisible);

	void DropWeaponState();
	void EquipWeaponState();


public:
	virtual bool Attack(FVector& HitLocation, bool IsRun, bool IsNotSpread);

	virtual void Reload();
	virtual bool CanReload();

	virtual bool GetCanShooting();

	virtual FVector GetUpVector();

	float GetRecoilRoverTime(int32 Cnt);

	FVector GetRecoilCurveValue(float Time);

protected:
	UPROPERTY(EditAnywhere, Category = Weapon)
	FVector UpVector = FVector(0.f, 0.f, 0.f);


private:
	UPROPERTY()
	class USceneComponent* SceneComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = Item)
	class USkeletalMeshComponent* ItemMesh = nullptr;
	UPROPERTY(EditAnywhere, Category = Item)
	class USphereComponent* SphereCollision = nullptr;

	UPROPERTY(EditAnywhere, Category = Item)
	class UWidgetComponent* ItemInfoWidgetComponent = nullptr;
	UPROPERTY(EditAnywhere, Category = Item)
	TSubclassOf<class UUserWidget> ItemInfoWidgetClass;

	UPROPERTY(EditAnywhere, Category = Item)
	TSubclassOf<class UItemObject> ItemObjectClass;
	UPROPERTY()
	class UItemObject* ItemObject = nullptr;

	UPROPERTY(EditAnywhere, Category = Weapon)
	class UCurveVector* RecoilCurve = nullptr;


private:
	bool bHitByTrace = false;

	UPROPERTY(EditAnywhere, Category = Item)
	float SphereRadius = 100.f;

private:
	FVector PrevLocation;
	FVector CurrentLocation;
	UPROPERTY(EditAnywhere, Category = Drop)
	float StopSimulateTime = 1.5f;
	float StopCurrentSimulateTime = 0.f;

protected:
	UPROPERTY(EditAnywhere, Category = "Recoil")
	int32 StartSpreadShootCnt = 3;

	UPROPERTY(EditAnywhere, Category = "Recoil")
	float RecoilRecoveryForTime = 0.25f; // 총기회복에 필요한 시간

	// 언제부터 반동이 시작 되는지 (총기 딜레이 * 몇 발 부터 탄이 튀는지);
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float NeedForRecoilTime = 0.4f; // 총을 StartSpreadShootCnt 이하로 쏜 경우 반동 회복 시간이 짧음
	UPROPERTY(EditAnywhere, Category = "Recoil")
	float NeedForRecoilfullTime = 1.f;// 총을 StartSpreadShootCnt 넘게 쏜 경우 반동 회복 시간이 김

protected:
	UPROPERTY(EditAnywhere, Category = Shoot)
	float ShootingDelay = 0.3f;
	UPROPERTY(EditAnywhere, Category = Shoot)
	float Damage = 10.f;

	UPROPERTY(EditAnywhere, Category = Weapon)
	float RecoilScale = 5.f;

	UPROPERTY(EditAnywhere, Category = Weapon)
	float RunRecoilRadius = 50.f;
	UPROPERTY(EditAnywhere, Category = Weapon)
	float RecoilRadius = 10.f;

	float CurrentSlideLocation = 0.f;
	float TargetSlidLocation = 0.f;

private:
	UPROPERTY(EditAnywhere, Category = Weapon)
	FName GunMagName = FName("Clip_Bone");

public:
	FORCEINLINE class USceneComponent* GetSceneComponent() const { return SceneComponent; }
	FORCEINLINE class USphereComponent* GetSphereCollision() const { return SphereCollision; }
	FORCEINLINE float GetShootingDelay() const { return ShootingDelay; }
	FORCEINLINE float GetDamage() const { return Damage; }
	FORCEINLINE float GetRecoilScale() const { return RecoilScale; }
	FORCEINLINE class UItemObject* GetItemObject() const { return ItemObject; }
	FORCEINLINE void SetIsHitByTrace(bool bTrace) { bHitByTrace = bTrace; }
	FORCEINLINE class USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE float GetRecoilRecoveryForTime() const { return RecoilRecoveryForTime; }
	FORCEINLINE float GetNeedForRecoilTime() const { return NeedForRecoilTime; }
	FORCEINLINE float GetNeedForRecoilfullTime() const { return NeedForRecoilfullTime; }
	FORCEINLINE int32 GetStartSpreadShootCnt() const { return StartSpreadShootCnt; }
	FORCEINLINE FName GetGunMagName() const { return GunMagName; }
	FORCEINLINE float GetSlideLocation() { return CurrentSlideLocation; }
};
