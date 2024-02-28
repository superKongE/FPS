#include "FPS/Casing/Casing.h"
#include "Components/StaticMeshComponent.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = false;

	Casing = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Casing"));
	Casing->SetSimulatePhysics(true);
	Casing->SetEnableGravity(true);
}
void ACasing::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ACasing::DestroyTimerEnd, DestroyTimerDelay, false);
}

void ACasing::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ACasing::DestroyTimerEnd()
{
	Destroy();
}