#include "FPS/Item/Rifle.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/DecalComponent.h"

#include "FPS/FPSCharacter.h"
#include "FPS/Casing/Casing.h"


ARifle::ARifle()
{
	CurrentAmmo = MaxAmmo;

	UpVector = FVector(10.f, 0.f, 10.f);

	GetItemMesh()->BodyInstance.bLockYTranslation = true;
	GetItemMesh()->BodyInstance.bLockZRotation = true;
}
void ARifle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentSlideLocation = FMath::FInterpTo(CurrentSlideLocation, TargetSlidLocation, DeltaTime, 15.f);
}

void ARifle::TraceUnderMuzzle(FVector& HitLocation, bool IsRun, bool IsNotSpread)
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
		if(MuzzleParticle)
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleParticle, HitResult.ImpactPoint);
		if (BulletDecal)
		{
			UGameplayStatics::SpawnDecalAtLocation(this, BulletDecal, BulletDecalSize, HitResult.ImpactPoint, UKismetMathLibrary::MakeRotFromX(HitResult.Normal), BulletDecalLifeTime);
		}
	}
}
bool ARifle::Attack(FVector& HitLocation, bool IsRun, bool IsNotSpread)
{
	if (CurrentAmmo == 0) return false;
	
	TargetSlidLocation = -15.f;
	CurrentAmmo--;

	SpawnCasing();

	FTransform MuzzleTransform = GetItemMesh()->GetSocketTransform(FName("Muzzle"));
	// √—º“∏Æ, ¿Ã∆Â∆Æ ¿Áª˝
	if(MuzzleParticle)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleParticle, MuzzleTransform);
	if(ShootSound)
		UGameplayStatics::SpawnSoundAtLocation(this, ShootSound, MuzzleTransform.GetLocation());

	TraceUnderMuzzle(HitLocation, IsRun, IsNotSpread);

	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &ARifle::FireTimerEnd, ShootingDelay / 2.f, false);

	return true;
}


void ARifle::Reload()
{
	CurrentAmmo = MaxAmmo;
}
bool ARifle::CanReload()
{
	if (CurrentAmmo == MaxAmmo) return false;

	return true;
}


bool ARifle::GetCanShooting()
{
	if (CurrentAmmo == 0) return false;

	return true;
}


void ARifle::SpawnCasing()
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


void ARifle::FireTimerEnd()
{
	TargetSlidLocation = 0.f;
}


FVector ARifle::GetUpVector()
{
	return UpVector;
}