#include "FPS/Item/AK47.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DecalComponent.h"

#include "FPS/FPSCharacter.h"
#include "FPS/Casing/Casing.h"


AAK47::AAK47()
{
	CurrentAmmo = MaxAmmo;

	UpVector = FVector(0.f, -10.f, 10.f);

	GetItemMesh()->BodyInstance.bLockYTranslation = true;
	GetItemMesh()->BodyInstance.bLockZRotation = true;
}
void AAK47::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentSlideLocation = FMath::FInterpTo(CurrentSlideLocation, TargetSlidLocation, DeltaTime, 15.f);
}

void AAK47::TraceUnderMuzzle(FVector& HitLocation, bool IsRun, bool IsNotSpread)
{
	AFPSCharacter* OwnerCharacter = Cast<AFPSCharacter>(GetOwner());
	if (OwnerCharacter == nullptr) return;

	const USkeletalMeshSocket* MuzzleSocket = GetItemMesh()->GetSocketByName(FName("Muzzle"));
	if (MuzzleSocket == nullptr) return;

	FVector Start = MuzzleSocket->GetSocketLocation(GetItemMesh());
	FVector Direction;

	FVector RandUnitVector = UKismetMathLibrary::RandomUnitVector();
	// √—æÀ¿Ã ¿ß∑Œ∏∏ ∆¢µµ∑œ
	if (RandUnitVector.Z < 0.f)
		RandUnitVector.Z *= -1.f;

	if (IsRun)
	{
		Direction = (HitLocation + RandUnitVector * RunRecoilRadius) - Start;
	}
	else
	{
		if (!IsNotSpread)
			Direction = (HitLocation + RandUnitVector * RecoilRadius) - Start;
		else
			Direction = HitLocation - Start;
	}
	Direction.Normalize();

	FVector End = Start + Direction * TraceDistance;

	FHitResult HitResult;
	FCollisionQueryParams FQP;
	FQP.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility, FQP);

	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, true);
	if (HitResult.bBlockingHit)
	{
		if (MuzzleParticle)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleParticle, HitResult.ImpactPoint);
		if (BulletDecal)
		{
			UGameplayStatics::SpawnDecalAtLocation(this, BulletDecal, BulletDecalSize, HitResult.ImpactPoint, UKismetMathLibrary::MakeRotFromX(HitResult.Normal), BulletDecalLifeTime);
		}
	}
}
bool AAK47::Attack(FVector& HitLocation, bool IsRun, bool IsNotSpread)
{
	if (CurrentAmmo == 0) return false;

	TargetSlidLocation = -6.f;
	CurrentAmmo--;

	SpawnCasing();

	FTransform MuzzleTransform = GetItemMesh()->GetSocketTransform(FName("Muzzle"));
	// √—º“∏Æ, ¿Ã∆Â∆Æ ¿Áª˝
	if (MuzzleParticle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleParticle, MuzzleTransform);
	if (ShootSound)
		UGameplayStatics::SpawnSoundAtLocation(this, ShootSound, MuzzleTransform.GetLocation());

	TraceUnderMuzzle(HitLocation, IsRun, IsNotSpread);

	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &AAK47::FireTimerEnd, ShootingDelay / 2.f, false);

	return true;
}


void AAK47::Reload()
{
	CurrentAmmo = MaxAmmo;
}
bool AAK47::CanReload()
{
	if (CurrentAmmo == MaxAmmo) return false;

	return true;
}

bool AAK47::GetCanShooting()
{
	if (CurrentAmmo == 0) return false;

	return true;
}


void AAK47::SpawnCasing()
{
	FTransform CasingTransform = GetItemMesh()->GetSocketTransform(FName("CasingPoint"));
	if (CasingClass)
	{
		ACasing* Casing = GetWorld()->SpawnActor<ACasing>(CasingClass, CasingTransform);
		if (Casing)
		{
			const FRotator YawRotation(0, CasingTransform.GetRotation().Rotator().Yaw, 0);

			const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			Casing->GetCasingMesh()->AddImpulse(Direction);
		}
	}
}


void AAK47::FireTimerEnd()
{
	TargetSlidLocation = 0.f;
}


FVector AAK47::GetUpVector()
{
	return UpVector;
}