#include "FPS/HUD/FPSHUD.h"
#include "Engine/Texture2D.h"


void AFPSHUD::BeginPlay()
{

}
void AFPSHUD::DrawHUD()
{
	FVector2D ViewportSize;
	FVector2D ViewportCenter;
	float CrosshairSpread = CrosshairInfo.CrosshairSpread;

	GEngine->GameViewport->GetViewportSize(ViewportSize);
	ViewportCenter = FVector2D(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);

	if (CrosshairInfo.bDraw)
	{
		DrawCrosshair(CrosshairInfo.LeftCrossHair, ViewportCenter, -CrosshairInfo.CrosshairSpread, 0.f);
		DrawCrosshair(CrosshairInfo.RightCrossHair, ViewportCenter, CrosshairInfo.CrosshairSpread, 0.f);
		DrawCrosshair(CrosshairInfo.UpCrossHair, ViewportCenter, 0.f, -CrosshairInfo.CrosshairSpread);
		DrawCrosshair(CrosshairInfo.DownCrossHair, ViewportCenter, 0.f, CrosshairInfo.CrosshairSpread);
		DrawCrosshair(CrosshairInfo.CenterCrossHair, ViewportCenter, 0.f, 0.f);
	}
}
void AFPSHUD::DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, float CrosshairSpreadX, float CrosshairSpreadY)
{
	const float TextureWidth = Texture->GetSizeX();
	const float TextureHeight = Texture->GetSizeY();
	const FVector2D DrawPoint(
		ViewportCenter.X - (TextureWidth / 2.f) + CrosshairSpreadX,
		ViewportCenter.Y - (TextureHeight / 2.f) + CrosshairSpreadY
	);

	DrawTexture(Texture, DrawPoint.X, DrawPoint.Y, Texture->GetSizeX(), Texture->GetSizeY(), 0.f, 0.f, 1.f, 1.f, FLinearColor::White);
}