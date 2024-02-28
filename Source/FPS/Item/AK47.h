#pragma once

#include "CoreMinimal.h"
#include "FPS/Item/Item.h"
#include "AK47.generated.h"



UCLASS()
class FPS_API AAK47 : public AItem
{
	GENERATED_BODY()
	
public:
	AAK47();

	virtual void Tick(float DeltaTime) override;

	bool Attack(FVector& HitLocation, bool IsRun, bool IsNotSpread);

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

	UPROPERTY(EditAnywhere, Category=Weapon)
	int32 MaxAmmo = 30.f;
	int32 CurrentAmmo;
	UPROPERTY(EditAnywhere, Category = Weapon)
	float TraceDistance = 8000.f;

	UPROPERTY(EditAnywhere, Category=Weapon)
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
