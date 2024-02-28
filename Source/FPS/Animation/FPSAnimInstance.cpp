#include "FPS/Animation/FPSAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "FPS/FPSCharacter.h"
#include "FPS/ActorComponent/CombatComponent.h"
#include "FPS/ActorComponent/FootIKComponent.h"

void UFPSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	FPSCharacter = Cast<AFPSCharacter>(GetOwningActor());
	if (FPSCharacter)
	{
		FPSCombatComponent = FPSCharacter->GetCombatComponent();
		FootIKComponent = FPSCharacter->GetFootIKComponent();
	}
}
void UFPSAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (FPSCharacter == nullptr || FPSCombatComponent == nullptr) return;

	Pitch = FPSCharacter->GetPitch();

	if (Pitch > 180.f)
		Pitch = 360.f - Pitch;
	else
		Pitch *= -1.f;
	Pitch /= 3.f;
	PitchRotation = FRotator(0.f, 0.f, Pitch);

	bEquipped = FPSCharacter->GetIsEquipped();
	LeftHandTransform = FPSCharacter->GetLeftHandTransform();
	bAimming = FPSCombatComponent->GetIsAimming();
	FowardRotation = FPSCharacter->GetActorForwardVector().Rotation();
	bUseLeftHandTransform = FPSCharacter->GetbUseLeftHandTransform();
	IsInAir = FPSCharacter->GetCharacterMovement()->IsFalling();

	HipOffset = FootIKComponent->GetHipOffset();
	LeftFootOffset = FootIKComponent->GetLeftFootOffset();
	RightFootOffset = FootIKComponent->GetRightFootOffset();

	LeftFootRotation = FootIKComponent->GetLeftFootRotation();
	RightFootRotation = FootIKComponent->GetRightFootRotation();
	
	CurrentRecoilTransform = FPSCombatComponent->GetCurrentRecoilTransform();
	HandTransform = FPSCombatComponent->GetHandTransform();

	PrevVelocity = Velocity;
	Velocity = FPSCharacter->GetVelocity();
	Direction = CalculateDirection(Velocity, FPSCharacter->GetActorRotation());
	Speed = Velocity.Size();
	

	if (-45.f - DeadZone <= Direction && Direction <= 45.f + DeadZone)
	{
		AnimationIndex = 0;
		OrientationDirection = 0.f;
	}
	else if (45.f - DeadZone <= Direction && Direction <= 135.f + DeadZone)
	{
		AnimationIndex = 1;
		OrientationDirection = 90.f;
	}
	else if (-135.f - DeadZone <= Direction && Direction <= -45.f + DeadZone)
	{
		AnimationIndex = 2;
		OrientationDirection = -90.f;
	}
	else if (-135.f + DeadZone >= Direction || 135.f - DeadZone <= Direction)
	{
		AnimationIndex = 3;
		OrientationDirection = 180.f;
	}

	Accelerate = (Velocity - PrevVelocity) / DeltaTime;
	IsAccelerating = Accelerate.Size() > 0.f;
}