#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FPSHUD.generated.h"

USTRUCT()
struct FCrossHairInfo
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	class UTexture2D* LeftCrossHair = nullptr;
	UPROPERTY(EditAnywhere)
	class UTexture2D* RightCrossHair = nullptr;
	UPROPERTY(EditAnywhere)
	class UTexture2D* UpCrossHair = nullptr;
	UPROPERTY(EditAnywhere)
	class UTexture2D* DownCrossHair = nullptr;
	UPROPERTY(EditAnywhere)
	class UTexture2D* CenterCrossHair = nullptr;

	UPROPERTY(EditAnywhere)
	float CrosshairSpread = 0.f;
	FLinearColor CrosshairColor;

	bool bDraw = true;
};

UCLASS()
class FPS_API AFPSHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;

	virtual void DrawHUD() override;
	void DrawCrosshair(class UTexture2D* Texture, FVector2D ViewportCenter, float CrosshairSpreadX, float CrosshairSpreadY);

private:
	UPROPERTY(EditAnywhere)
	FCrossHairInfo CrosshairInfo;

public:
	FORCEINLINE FCrossHairInfo* GetCrossHairInfo() { return &CrosshairInfo; }
};
