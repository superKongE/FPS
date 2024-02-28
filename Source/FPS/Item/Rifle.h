#pragma once

#include "CoreMinimal.h"
#include "FPS/Item/Item.h"
#include "Rifle.generated.h"


UCLASS()
class FPS_API ARifle : public AItem
{
	GENERATED_BODY()
		
public:
	ARifle();

	virtual void Tick(float DeltaTime) override;

	virtual bool Attack(FVector& HitLocation, bool IsRun, bool IsNotSpread) override;

	void TraceUnderMuzzle(FVector& HitLocation, bool IsRun, bool IsNotSpread);

	virtual void Reload();
	virtual bool CanReload();

	virtual bool GetCanShooting();

	virtual FVector GetUpVector();

	void SpawnCasing();

	void FireTimerEnd();

private:
	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<class ACasing> CasingClass;

	UPROPERTY(EditAnywhere, Category = Weapon)
	int32 MaxAmmo = 30.f;
	int32 CurrentAmmo;
	UPROPERTY(EditAnywhere, Category = Weapon)
	float TraceDistance = 8000.f;

	UPROPERTY(EditAnywhere, Category = Weapon)
	class UParticleSystem* HitParticle = nullptr;
	UPROPERTY(EditAnywhere, Category = Weapon)
	class UParticleSystem* MuzzleParticle = nullptr;
	UPROPERTY(EditAnywhere, Category = Weapon)
	class UParticleSystem* TraceParticle = nullptr;

	UPROPERTY(EditAnywhere, Category = Weapon)
	class USoundCue* ShootSound = nullptr;

	UPROPERTY(EditAnywhere, Category = Weapon)
	class UMaterialInstance* BulletDecal = nullptr;
	UPROPERTY(EditAnywhere, Category = Weapon)
	FVector BulletDecalSize = FVector(0.1f, 0.1f, 0.1f);
	UPROPERTY(EditAnywhere, Category = Weapon)
	float BulletDecalLifeTime = 2.f;


	FTimerHandle FireTimerHandle;
};
