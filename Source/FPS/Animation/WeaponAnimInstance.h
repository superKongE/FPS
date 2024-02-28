#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponAnimInstance.generated.h"


UCLASS()
class FPS_API UWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AItem* Item = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class AFPSCharacter* OwnerCharacter = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* OwnerCombatComponent = nullptr;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FTransform HandTransform;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bMovingClip;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float SlideLocation;
};
