#include "FPS/Animation/WeaponAnimInstance.h"

#include "FPS/FPSCharacter.h"
#include "FPS/Item/Item.h"
#include "FPS/ActorComponent/CombatComponent.h"


void UWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	Item = Cast<AItem>(GetOwningActor());
}
void UWeaponAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (Item)
	{
		OwnerCharacter = Cast<AFPSCharacter>(Item->GetOwner());
		if (OwnerCharacter)
		{
			OwnerCombatComponent = OwnerCharacter->GetCombatComponent();
			if (OwnerCombatComponent)
			{
				if(OwnerCombatComponent->GetHandSceneComponent())
					HandTransform = OwnerCombatComponent->GetHandSceneComponent()->GetComponentTransform();
				bMovingClip = OwnerCombatComponent->GetbMovingGumMag();
				SlideLocation = Item->GetSlideLocation();
			}
		}
	}
}