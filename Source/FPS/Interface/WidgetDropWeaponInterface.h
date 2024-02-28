#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WidgetDropWeaponInterface.generated.h"


DECLARE_DELEGATE_OneParam(FWeaponDropDelegateInterface, class UItemObject*)


UINTERFACE(MinimalAPI)
class UWidgetDropWeaponInterface : public UInterface
{
	GENERATED_BODY()

public:
	FWeaponDropDelegateInterface WeaponDropDelegateInterface;

	//virtual FWeaponDropDelegateInterface GetDelegate() = 0;
};

/**
 * 
 */
class FPS_API IWidgetDropWeaponInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
};
