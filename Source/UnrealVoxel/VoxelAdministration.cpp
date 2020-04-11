// Copyright 2016-2017 Tefel. All Rights Reserved.

#include "VoxelAdministration.h"
#include "UnrealVoxel.h"

// Sets default values
AVoxelAdministration::AVoxelAdministration()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AVoxelAdministration::UpdateVoxel(int32 SectorXindex, int32 SectorYindex, FVoxel Voxel)
{
	TArray<FSectorArrayY2> & sectorsY = (SectorYindex < 0) ? sectorsArrYneg : sectorsArrY;
	SectorYindex = abs(SectorYindex);
	if (SectorYindex >= sectorsY.Num())
	{
		sectorsY.SetNum(SectorYindex + 1);
		sectorsY[SectorYindex] = FSectorArrayY2();

	}
	FSectorArrayY2 & sectorArrY = sectorsY[SectorYindex];

	TArray<FSectorX> & sectorsX = (SectorXindex < 0) ? sectorArrY.SectorsXneg : sectorArrY.SectorsX;
	SectorXindex = abs(SectorXindex);
	if (SectorXindex >= sectorsX.Num())
	{
		sectorsX.SetNum(SectorXindex + 1);
		sectorsX[SectorXindex] = FSectorX();
	}
	FSectorX & sectorX = sectorsX[SectorXindex];
	
	int32 searchIndex = FindVoxelIndex(sectorX.Voxels, Voxel.Index);

	if (searchIndex != -1) // voxel already exist, modify
	{
		sectorX.Voxels[searchIndex] = Voxel;
	}
	else // voxel not exist, add new
	{
		sectorX.Voxels.Add(Voxel);
	}
}

int32 AVoxelAdministration::FindVoxelIndex(const TArray<FVoxel>& voxels, int32 Index)
{
	for (int32 i = 0; i < voxels.Num(); i++)
	{
		if (voxels[i].Index == Index) return i;
	}
	return -1;
}

FSectorX AVoxelAdministration::getVoxelData(int32 SectorXindex, int32 SectorYindex)
{
	TArray<FSectorArrayY2> & sectorsY = (SectorYindex < 0) ? sectorsArrYneg : sectorsArrY;
	SectorYindex = abs(SectorYindex);
	if (SectorYindex < sectorsY.Num())
	{
		FSectorArrayY2 & sectorArrY = sectorsY[SectorYindex];
		TArray<FSectorX> & sectorsX = (SectorXindex < 0) ? sectorArrY.SectorsXneg : sectorArrY.SectorsX;
		SectorXindex = abs(SectorXindex);

		if (SectorXindex < sectorsX.Num())
		{
			return sectorsX[SectorXindex];
		}
	}
	return FSectorX();
}

