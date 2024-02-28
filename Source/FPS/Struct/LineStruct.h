#pragma once

#include "CoreMinimal.h"
#include "LineStruct.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FLineStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D Start;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D End;
};

USTRUCT(Atomic, BlueprintType)
struct FTileStruct
{
	GENERATED_USTRUCT_BODY()
public:
	FTileStruct() : X(0), Y(0) {}
	FTileStruct(int32 X_, int32 Y_) : X(X_), Y(Y_) {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 X;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Y;
};

class FPS_API LineStruct
{
public:
	LineStruct();
	~LineStruct();
};
