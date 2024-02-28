#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

UCLASS(config=Game)
class AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	bool bActiveInventory = false;

public:
	AFPSCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

protected:
	void BeginPlay() override;
	void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);



public:
	void ToggleInventory();

	void ToggleSpeed();


	void GetItem();
	void EquipWeaponFromSlot(class UItemObject* ItemObject, int32 Index);

	void DropWeapon();

	void ChangeToFirstWeapon();
	void ChangeToSecondWeapon();


	void Aim();

	void ChangeCamera();

	class UCameraComponent* GetCurrentCamera();

	void ShootPressed();
	void ShootReleased();

	void Reload();

	void SetEquippedWeapon(class AItem* Weapon);

	void AddRotationYaw(float Yaw);

	UFUNCTION(BlueprintCallable)
	void FootStepNotify(const FName& FootName);

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, Category = Camera)
	class UCameraComponent* FirstPersonCamera;
	UPROPERTY(EditAnywhere, Category=Camera)
	class UCameraComponent* ThirdPersonCamera = nullptr;

	UPROPERTY(EditAnywhere)
	class UCombatComponent* CombatComponent = nullptr;

	UPROPERTY(EditAnywhere)
	class UInventoryComponent* InventoryComponent = nullptr;

	UPROPERTY(EditAnywhere)
	class UFootIKComponent* FootIKComponent = nullptr;

	UPROPERTY()
	class APlayerController* FPSController = nullptr;
	UPROPERTY()
	class AFPSHUD* FPSHUD = nullptr;

	UPROPERTY(EditAnywhere)
	class USceneComponent* WeaponDropPoint = nullptr;

	UPROPERTY()
	class ARenderTargetCharacter* RenderTargetCharacter = nullptr;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ARenderTargetCharacter> RenderTargetCharacterClass;

	UPROPERTY(EditAnywhere, Category = FootStep)
	class UNiagaraSystem* WoodParticle = nullptr;
	UPROPERTY(EditAnywhere, Category = FootStep)
	class UNiagaraSystem* StoneParticle = nullptr;
	UPROPERTY(EditAnywhere, Category = FootStep)
	class UNiagaraSystem* SandParticle = nullptr;
	UPROPERTY(EditAnywhere, Category = FootStep)
	class USoundCue* WoodSound = nullptr;
	UPROPERTY(EditAnywhere, Category = FootStep)
	class USoundCue* StoneSound = nullptr;
	UPROPERTY(EditAnywhere, Category = FootStep)
	class USoundCue* SandSound = nullptr;


private:
	UPROPERTY(EditAnywhere, Category=FootStep)
	float FootStepLineTraceDistance = 10.f;

private:
	UPROPERTY()
	TArray<class AItem*> WeaponArray;
	UPROPERTY()
	TArray<FName> WeaponSokcetNameArray;
	int32 EquippedWeaponCnt = 0;
	int32 CurrentEquippedWeaponIndex = -1;
	bool bEquipped = false;
	FTransform LeftHandTransform;

	bool bUseLeftHandTransform = false;

private:
	float Pitch;
	bool bJog = false;

	UPROPERTY(EditAnywhere, Category = Combat)
	float WalkSpeed = 300;
	UPROPERTY(EditAnywhere, Category = Combat)
	float JogSpeed = 500.f;

private:
	bool bFirstPersonCamera = false;

private:
	bool bInventoryOpen = false;

public:
	FORCEINLINE class UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	FORCEINLINE class ARenderTargetCharacter* GetRenderTargetCharacter() const { return RenderTargetCharacter; }
	FORCEINLINE class UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE class APlayerController* GetFPSController() const { return FPSController; }
	FORCEINLINE class UFootIKComponent* GetFootIKComponent() const { return FootIKComponent; }
	FORCEINLINE class AFPSHUD* GetFPSHUD() const { return FPSHUD; }
	FORCEINLINE float GetPitch() const { return Pitch; }
	FORCEINLINE void SetPitch(float Pitch_) { Pitch += Pitch_; }
	FORCEINLINE float GetWalkSpeed() const { return WalkSpeed; }
	FORCEINLINE TArray<class AItem*>& GetWeaponArray() { return WeaponArray; }
	FORCEINLINE TArray<FName>& GetWeaponSocketNameArray() { return WeaponSokcetNameArray; }
	FORCEINLINE int32& GetEquippedWeaponCnt() { return EquippedWeaponCnt; }
	FORCEINLINE int32& GetCurrentEquippedWeaponIndex()  { return CurrentEquippedWeaponIndex; }
	FORCEINLINE void SetEquippedWeaponCnt(int32 Add) { EquippedWeaponCnt += Add; }
	FORCEINLINE void SetCurrentEquippedWeaponIndex(int32 Index) { CurrentEquippedWeaponIndex = Index; }
	FORCEINLINE bool GetIsEquipped() const { return bEquipped; }
	FORCEINLINE bool GetIsJogging() const { return bJog; }
	FORCEINLINE void SetIsEquipped(bool bEquipped_) { bEquipped = bEquipped_; }
	FORCEINLINE FTransform GetLeftHandTransform() const { return LeftHandTransform; }
	FORCEINLINE FVector GetWeaponDropPoint() const { return WeaponDropPoint->GetComponentLocation(); }
	FORCEINLINE bool GetIsInventoryOpen() const { return bInventoryOpen; }
	FORCEINLINE bool GetbUseLeftHandTransform() const { return bUseLeftHandTransform; }
	FORCEINLINE void SetbUseLeftHandTransform(bool bUse) { bUseLeftHandTransform = bUse; }
	FORCEINLINE bool GetIsFirstPersonCamera() const { return bFirstPersonCamera; }
};

