#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemInfoWidget.generated.h"


UCLASS()
class FPS_API UItemInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(class UMaterialInterface* ItemMaterial);
	
private:
	UPROPERTY(meta = (BindWidget, AllowPrivateAccess = "true"))
	class UImage* ItemImage = nullptr;
};
