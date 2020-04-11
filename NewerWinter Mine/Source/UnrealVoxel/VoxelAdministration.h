// Copyright 2016-2017 Tefel. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "VoxelAdministration.generated.h"

USTRUCT(BlueprintType)
struct FVoxel
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	int32 Index = -1;

	UPROPERTY(BlueprintReadWrite)
	uint8 Value = 0;

	UPROPERTY(BlueprintReadWrite)
	uint8 Red = 0;

	UPROPERTY(BlueprintReadWrite)
	uint8 Green = 0;

	UPROPERTY(BlueprintReadWrite)
	uint8 Blue = 0;
};

/**
*	Sector element - X coordinate
*/
USTRUCT(BlueprintType)
struct FSectorX
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FVoxel> Voxels;

};

/**
*	Struct represents sectors arrays - Y lines (contains X elements)
*/
USTRUCT(BlueprintType)
struct FSectorArrayY2
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FSectorX> SectorsX;

	UPROPERTY(BlueprintReadWrite)
	TArray<FSectorX> SectorsXneg;
};


UCLASS()
class UNREALVOXEL_API AVoxelAdministration : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVoxelAdministration();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool settingMaterial = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool settingObjectsStatic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool settingObjectsDynamic = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSectorArrayY2> sectorsArrY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSectorArrayY2> sectorsArrYneg;

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	int32 FindVoxelIndex(const TArray<FVoxel>& voxels, int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	void UpdateVoxel(int32 SectorXindex, int32 SectorYindex, FVoxel BubbleVoxel);

	UFUNCTION(BlueprintCallable, Category = "Voxel")
	FSectorX getVoxelData(int32 SectorXindex, int32 SectorYindex);

};
