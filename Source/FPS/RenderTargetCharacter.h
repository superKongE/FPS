#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RenderTargetCharacter.generated.h"

UCLASS()
class FPS_API ARenderTargetCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARenderTargetCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void Init(class AFPSCharacter* FPSCharacter);

	void EquipWeapon(TSubclassOf<class AItem> ItemClass);
	void DropWeapon();
	void AttachWeaponBack(TSubclassOf<class AItem> ItemClass);
	void EquipNextWeapon();

	void AddRotationYaw(float Yaw);

private:
	UPROPERTY(EditAnywhere, Category = RenderTarget)
	TSubclassOf<class ACharacterCapture2D> SceneCapture2DClass = nullptr;
	UPROPERTY(EditAnywhere, Category=RenderTarget)
	class ACharacterCapture2D* SceneCapture2D = nullptr;

	UPROPERTY(EditAnywhere, Category = RenderTarget)
	class UArrowComponent* SceneCaptrueSpawnLocation = nullptr;

	UPROPERTY()
	class AItem* EquippedWeapon = nullptr;
	UPROPERTY()
	class AItem* EquippedBackWeapon = nullptr;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> ShowOnlyActors;

	bool bEquipped = false;
	FVector LeftHandTransform;

public:
	FORCEINLINE bool GetIsEquipped() const { return bEquipped; }
	FORCEINLINE FVector GetLeftHandTransform() const { return LeftHandTransform; }
};
