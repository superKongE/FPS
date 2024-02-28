#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class FPS_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	ACasing();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void DestroyTimerEnd();

private:
	UPROPERTY(EditAnywhere, Category=Casing)
	class UStaticMeshComponent* Casing = nullptr;

	UPROPERTY(EditAnywhere, Category = Casing)
	float DestroyTimerDelay = 2.f;
	FTimerHandle DestroyTimerHandle;

public:
	FORCEINLINE UStaticMeshComponent* GetCasingMesh() const { return Casing; }
};
