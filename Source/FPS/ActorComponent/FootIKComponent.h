// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FootIKComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FPS_API UFootIKComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UFootIKComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init(class AFPSCharacter* Chr);
		
	void LineTraceUnderFoot();
	float LeftFootLineTrace(FTransform LeftFootTransform);
	float RightFootLineTrace(FTransform RightFootTransform);

	FRotator NormalToRotator(FVector pVector);

private:
	UPROPERTY(EditAnywhere, Category = FootIK)
	class AFPSCharacter* OwnerCharacter = nullptr;

	UPROPERTY(EditAnywhere, Category = FootIK)
	FName LeftFootName = FName("foot_l_socket");
	UPROPERTY(EditAnywhere, Category = FootIK)
	FName RightFootName = FName("foot_r_socket");

	float InitCapsuleHalfHeight;

	float TraceDistance;

	FCollisionQueryParams FQP;

	float CurrentHipOffset = 0.f;;
	float TargetHipOffset;

	float CurrentLeftFootOffset = 0.f;
	float TargetLeftFootOffset;

	float CurrentRightFootOffset = 0.f;
	float TargetRightFootOffset;

	FRotator LeftFootRotation;
	FRotator RightFootRotation;

public:
	FORCEINLINE float GetHipOffset() const { return CurrentHipOffset; }
	FORCEINLINE float GetLeftFootOffset() const { return CurrentLeftFootOffset; }
	FORCEINLINE float GetRightFootOffset() const { return CurrentRightFootOffset; }
	FORCEINLINE FRotator GetLeftFootRotation() const { return LeftFootRotation; }
	FORCEINLINE FRotator GetRightFootRotation() const { return RightFootRotation; }
};
