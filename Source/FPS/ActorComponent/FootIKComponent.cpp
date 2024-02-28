#include "FPS/ActorComponent/FootIKComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "FPS/FPSCharacter.h"

UFootIKComponent::UFootIKComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}
void UFootIKComponent::BeginPlay()
{
	Super::BeginPlay();

}
void UFootIKComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwnerCharacter == nullptr) return;

	LineTraceUnderFoot();

	CurrentHipOffset = FMath::FInterpTo(CurrentHipOffset, TargetHipOffset, DeltaTime, 15.f);
	OwnerCharacter->GetCapsuleComponent()->SetCapsuleHalfHeight(InitCapsuleHalfHeight + CurrentHipOffset * 0.5f);

	CurrentLeftFootOffset = FMath::FInterpTo(CurrentLeftFootOffset, TargetLeftFootOffset, DeltaTime, 15.f);
	CurrentRightFootOffset = FMath::FInterpTo(CurrentRightFootOffset, TargetRightFootOffset, DeltaTime, 15.f);
}
void UFootIKComponent::Init(AFPSCharacter* Chr)
{
	if (Chr == nullptr) return;

	OwnerCharacter = Chr;

	InitCapsuleHalfHeight = OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	TraceDistance = InitCapsuleHalfHeight;

	FQP.AddIgnoredActor(OwnerCharacter);
}


void UFootIKComponent::LineTraceUnderFoot()
{
	FTransform LeftFootTransform = OwnerCharacter->GetMesh()->GetSocketTransform(LeftFootName);
	FTransform RightFootTransform = OwnerCharacter->GetMesh()->GetSocketTransform(RightFootName);

	float LeftFootResult = LeftFootLineTrace(LeftFootTransform);
	float RightFootResult = RightFootLineTrace(RightFootTransform);

	TargetHipOffset = 0.f;

	// 왼발이 올라가는 경우
	if (LeftFootResult >= RightFootResult)
	{
		TargetHipOffset = RightFootResult;
		TargetLeftFootOffset = TargetHipOffset;
		TargetRightFootOffset = 0.f;
	}
	else
	{
		TargetHipOffset = LeftFootResult;
		TargetLeftFootOffset = 0.f;
		TargetRightFootOffset = TargetHipOffset;
	}
}
float UFootIKComponent::LeftFootLineTrace(FTransform LeftFootTransform)
{
	FVector LeftFootStart = LeftFootTransform.GetLocation();
	LeftFootStart.Z += InitCapsuleHalfHeight / 2.f;

	FVector LeftFootEnd = LeftFootStart - FVector(0.f, 0.f, TraceDistance);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, LeftFootStart, LeftFootEnd, ECollisionChannel::ECC_WorldStatic, FQP);
	//DrawDebugLine(GetWorld(), LeftFootStart, LeftFootEnd, FColor::Red, false);

	if (HitResult.bBlockingHit)
	{
		if (OwnerCharacter->GetVelocity().Size() == 0.f)
		{
			LeftFootRotation = NormalToRotator(HitResult.Normal);
		}
		/*else
		{
			RightFootRotation = FRotator::ZeroRotator;
		}*/

		return HitResult.ImpactPoint.Z - OwnerCharacter->GetMesh()->GetComponentLocation().Z;
	}
	
	return 0.f;
}
float UFootIKComponent::RightFootLineTrace(FTransform RightFootTransform)
{
	FVector RightFootStart = RightFootTransform.GetLocation();
	RightFootStart.Z += InitCapsuleHalfHeight / 2.f;

	FVector RightFootEnd = RightFootStart - FVector(0.f, 0.f, TraceDistance);

	FHitResult HitResult;
	GetWorld()->LineTraceSingleByChannel(HitResult, RightFootStart, RightFootEnd, ECollisionChannel::ECC_WorldStatic, FQP);
	//DrawDebugLine(GetWorld(), RightFootStart, RightFootEnd, FColor::Red, false);

	if (HitResult.bBlockingHit)
	{
		if (OwnerCharacter->GetVelocity().Size() == 0.f)
		{
			RightFootRotation = NormalToRotator(HitResult.Normal);
		}
		/*else
		{
			RightFootRotation = FRotator::ZeroRotator;
		}*/

		return HitResult.ImpactPoint.Z - OwnerCharacter->GetMesh()->GetComponentLocation().Z;
	}

	return 0.f;
}


FRotator UFootIKComponent::NormalToRotator(FVector pVector)
{
	float fAtan2_1 = UKismetMathLibrary::DegAtan2(pVector.Y, pVector.Z);
	float fAtan2_2 = UKismetMathLibrary::DegAtan2(pVector.X, pVector.Z);
	fAtan2_2 *= -1.0f;
	FRotator pResult = FRotator(fAtan2_2, 0.0f, fAtan2_1);

	return pResult;
}