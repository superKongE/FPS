#include "FPS/Widget/ItemInfoWidget.h"
#include "Components/Image.h"

void UItemInfoWidget::Init(UMaterialInterface* ItemMaterial)
{
	if (ItemImage && ItemMaterial)
	{
		ItemImage->SetBrushFromMaterial(ItemMaterial);
	}
}