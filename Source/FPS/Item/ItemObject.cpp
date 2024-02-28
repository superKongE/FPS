#include "FPS/Item/ItemObject.h"

#include "Materials/MaterialInterface.h"

UItemObject::UItemObject()
{
	Dimensions.X = IconWidth;
	Dimensions.Y = IconHeight;

	Icon = CreateDefaultSubobject<UMaterialInstance>(TEXT("Icon"));
	IconRotated = CreateDefaultSubobject<UMaterialInstance>(TEXT("IconRotated"));
}


UMaterialInstance* UItemObject::GetIcon()
{
	if (bRotated && IconRotated)
	{
		return IconRotated;
	}
	else if(Icon)
	{
		return Icon;
	}

	return nullptr;
}
FIntPoint UItemObject::GetDimensions()
{
	if (bRotated)
	{
		Dimensions.X = IconHeight;
		Dimensions.Y = IconWidth;
	}
	else
	{
		Dimensions.X = IconWidth;
		Dimensions.Y = IconHeight;
	}

	return Dimensions;
}
FIntPoint UItemObject::GetUnRotateDimensions()
{
	Dimensions.X = IconWidth;
	Dimensions.Y = IconHeight;

	return Dimensions;
}


void UItemObject::Rotate()
{
	bRotated = !bRotated;
}