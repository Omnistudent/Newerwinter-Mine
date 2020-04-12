// Copyright 2016-2017 Tefel. All Rights Reserved.

#include "VoxelActor.h"
#include "UnrealVoxel.h"
#include "SimplexNoiseLibrary.h"
#include "VoxelGameInstance.h"
#include "EngineUtils.h"

const int32 bTriangles[] = { 2, 1, 0, 0, 3, 2 };
const FVector2D bUVs[] = { FVector2D(0.000000, 0.000000), FVector2D(0.000000, 1.000000), FVector2D(1.000000, 1.000000), FVector2D(1.000000, 0.000000) };
const FVector bNormals0[] = { FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1), FVector(0, 0, 1) };
const FVector bNormals1[] = { FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1), FVector(0, 0, -1) };
const FVector bNormals2[] = { FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0), FVector(0, 1, 0) };
const FVector bNormals3[] = { FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0), FVector(0, -1, 0) };
const FVector bNormals4[] = { FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0), FVector(1, 0, 0) };
const FVector bNormals5[] = { FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0), FVector(-1, 0, 0) };
const FVector bMask[] = { FVector(0.000000, 0.000000, 1.000000), FVector(0.000000, 0.000000, -1.000000), FVector(0.000000, 1.000000, 0.000000), FVector(0.000000, -1.000000, 0.000000), FVector(1.000000, 0.000000, 0.000000), FVector(-1.000000, 0.000000, 0.000000) };

// Sets default values
AVoxelActor::AVoxelActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

 void AVoxelActor::OnConstruction(const FTransform & Transform)
{
	UVoxelGameInstance * gameInstance;
	if (GetGameInstance() != nullptr)
	{
		gameInstance = Cast<UVoxelGameInstance>(GetGameInstance());
		randomSeed = gameInstance->randomSeed;
		//firstmultiplier = gameInstance->randomSeed;
		firstmultiplier = gameInstance->firstmultiplier;
		//multiplier2 = gameInstance->multiplier2;
		//firstmultiplier = 16.0;
	
		
		if (gameInstance->customSettings.Num() > 0 && gameInstance->customSettings[3] == 1)
		{
			Materials[0] = leavesMaterialRounded; // replace material for rounded one
		}

		
		
		

		
	}
	for (TActorIterator<AVoxelAdministration> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		voxelAdministration = *ActorItr;
	}

	USimplexNoiseLibrary::setNoiseSeed(randomSeed); // random seed for noise

	chunkZElements = 64;
	chunkLineElementsExt = chunkLineElements + 2;
	chunkTotalElements = chunkLineElementsExt * chunkLineElementsExt * chunkZElements;
	chunkLineElementsP2 = chunkLineElements * chunkLineElements;
	chunkLineElementsP2Ext = chunkLineElementsExt * chunkLineElementsExt;
	voxelSizeHalf = voxelSize / 2;

	FString string = "Voxel_" + FString::FromInt(chunkXindex) + "_" + FString::FromInt(chunkYindex);
	FName name = FName(*string);
	proceduralComponent = NewObject<UProceduralMeshComponent>(this, name);
	proceduralComponent->RegisterComponent();

	RootComponent = proceduralComponent;
	RootComponent->SetWorldTransform(Transform); // set position of root component (bubble actor)

	Super::OnConstruction(Transform);

	if (voxelAdministration == nullptr) return;

	GenerateChunk();		// generate voxel data from noise
	UpdateChunkFromData();	// get saved voxel DATA	
	UpdateMesh();			// generate mesh sides
	UpdateExtras();			// generate extra instanced meshes and particles
}


void AVoxelActor::GenerateChunk()
{
	FRandomStream RandomStream = FRandomStream(randomSeed);
	TArray<FIntVector> treeCenters;
	TArray <int32> tempLeaves;

	chunkFields.SetNumUninitialized(chunkTotalElements);

	TArray <int32> noise = calculateNoise();
	
	for (int32 x = 0; x < chunkLineElementsExt; x++)
	{
		for (int32 y = 0; y < chunkLineElementsExt; y++)
		{
			for (int32 z = 0; z < chunkZElements; z++)
			{
				int32 index = x + (y * chunkLineElementsExt) + (z * chunkLineElementsP2Ext);

				//if (z == 31 + noise[x + y * chunkLineElements] && RandomStream.FRand() < 0.01) treeCenters.Add(FIntVector(x, y, z));
	
				if (z == 30 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = 11;
				else if (z == 29 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = 12;
				else if (z < 29 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = 13;
				else chunkFields[index] = 0;
				//int32 index = x + (y * chunkLineElements) + (z * chunkLineElementsP2);
				//chunkFields[index] = (z == 29 + noise[x + y * chunkLineElements]) ? 1 : 0;
				//if (z == 28 + noise[x + y * chunkLineElements]) chunkFields[index] = 2;
				//if (z < 28 + noise[x + y * chunkLineElements]) chunkFields[index] = 3;
				//if (RandomStream.FRand() < 0.01 && z == 30 + noise[x + y * chunkLineElements]) treeCenters.Add(FIntVector(x, y, z));
			}
		}
	}

	// smaller range for trees - we dont want to spawn them on shared areas or edges
	for (int32 x = 2; x < chunkLineElementsExt-2; x++)
	{
		for (int32 y = 2; y < chunkLineElementsExt-2; y++)
		{
			for (int32 z = 0; z < chunkZElements; z++)
			{
				int32 index = x + (y * chunkLineElementsExt) + (z * chunkLineElementsP2Ext);
				if (RandomStream.FRand() < 0.06 && z == 31 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = -1; // grass
				if (RandomStream.FRand() < 0.01 && z == 31 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = -2; // flower1
				if (RandomStream.FRand() < 0.01 && z == 31 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = -3; // flower2
				if (RandomStream.FRand() < 0.01 && z == 31 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = -4; // flower3
				if (RandomStream.FRand() < 0.004 && z == 31 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = -5; // flower4								
				if (RandomStream.FRand() < 0.002 && z == 31 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = -7; // mushroom
				if (RandomStream.FRand() < 0.01 && z == 31 + noise[x + y * chunkLineElementsExt]) treeCenters.Add(FIntVector(x, y, z)); // trees
				if (RandomStream.FRand() < 0.001 && z == 32 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = -8; // butterfly
				if (RandomStream.FRand() < 0.0002 && z == 33 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = -9; // birds
				if (RandomStream.FRand() < 0.02 && z == 31 + noise[x + y * chunkLineElementsExt]) chunkFields[index] = -6; // sapling
				
			}
		}
	}
	
	for (FIntVector treeCenter : treeCenters)
	{
		int32 tree_heigh = RandomStream.RandRange(3, 6);
		int32 randomX = RandomStream.RandRange(0, 2);
		int32 randomY = RandomStream.RandRange(0, 2);
		int32 randomZ = RandomStream.RandRange(0, 2);

		int32 leavesVoxelCount = 0;
		// tree leaves
		for (int32 tree_x = -2; tree_x < 3; tree_x++)
		{
			for (int32 tree_y = -2; tree_y < 3; tree_y++)
			{
				for (int32 tree_z = -2; tree_z < 3; tree_z++)
				{
			
					if (inRange(tree_x + treeCenter.X + 1, chunkLineElements + 1) && inRange(tree_y + treeCenter.Y + 1, chunkLineElements + 1) && inRange(tree_z + treeCenter.Z + tree_heigh + 1, chunkZElements))
					{
						float radius = FVector(tree_x * randomX, tree_y * randomY, tree_z * randomZ).Size();

						if (radius <= 2.8)
							if (RandomStream.FRand() < 0.8 || radius <= 1.2)
							{
								//float treeCond = USimplexNoiseLibrary::SimplexNoise3D((sectorXindex * PlanetSize + treeCenter.x + tree_x)*xMult, (sectorYindex * PlanetSize + treeCenter.y + tree_y)*yMult, (sectorYindex * PlanetZSize + treeCenter.z + tree_z)*yMult) * weight;
								//if (treeCond > startCond-0.2 && treeCond < startCond + 0.2)								
								tempLeaves.Add(treeCenter.X + tree_x + (chunkLineElementsExt * (treeCenter.Y + tree_y)) + (chunkLineElementsP2Ext * (treeCenter.Z + tree_z + tree_heigh)));	// add to temp								
								leavesVoxelCount++;
							}	
					}
				}
			}
		}

		// for more leaves just smaller radius
		if (leavesVoxelCount > 110)
		{
			for (int32 tree_x = -2; tree_x < 3; tree_x++)
			{
				for (int32 tree_y = -2; tree_y < 3; tree_y++)
				{
					for (int32 tree_z = -2; tree_z < 3; tree_z++)
					{						
						if (FVector(tree_x, tree_y, tree_z).Size() <= (RandomStream.FRand() + 1.2f))
							chunkFields[treeCenter.X + tree_x + (chunkLineElementsExt * (treeCenter.Y + tree_y)) + (chunkLineElementsP2Ext * (treeCenter.Z + tree_z + tree_heigh))] = 1;
					}
				}
			}
		}
		else
		{
			for (int32 index : tempLeaves)
			chunkFields[index] = 1;
		}

		// tree trunk
		for (int32 h = 0; h < tree_heigh; h++)
		{
			chunkFields[treeCenter.X + (treeCenter.Y * chunkLineElementsExt) + ((treeCenter.Z + h) * chunkLineElementsP2Ext)] = 14;
		}
		if (RandomStream.FRand() < 0.4 && leavesVoxelCount > 10) chunkFields[treeCenter.X + (treeCenter.Y * chunkLineElementsExt) + ((treeCenter.Z + tree_heigh) * chunkLineElementsP2Ext)] = -10; // particle leaves
	}
}


void AVoxelActor::UpdateChunkFromData()
{
	FSectorX sectorX = voxelAdministration->getVoxelData(chunkXindex, chunkYindex);	// get all values from saved
	for (const FVoxel& voxel : sectorX.Voxels)
		chunkFields[voxel.Index] = voxel.Value;
}

void AVoxelActor::UpdateMesh()
{
	TArray<FMeshSection> meshSections;
	meshSections.SetNum(Materials.Num());
	int32 el_num = 0;


	for (int32 x = 0; x < chunkLineElements; x++)
	{
		for (int32 y = 0; y < chunkLineElements; y++)
		{
			for (int32 z = 0; z < chunkZElements; z++)
			{
				int32 index = (x + 1) + (chunkLineElementsExt * (y + 1)) + (chunkLineElementsP2Ext * z);
				int32 meshIndex = chunkFields[index];


				if (meshIndex > 0)
				{
					meshIndex--;

					TArray<FVector> &Vertices = meshSections[meshIndex].Vertices;
					TArray<int32> &Triangles = meshSections[meshIndex].Triangles;
					TArray<FVector> &Normals = meshSections[meshIndex].Normals;
					TArray<FVector2D> &UVs = meshSections[meshIndex].UVs;
					TArray<FProcMeshTangent> &Tangents = meshSections[meshIndex].Tangents;
					TArray<FColor> &VertexColors = meshSections[meshIndex].VertexColors;
					int32 elementID = meshSections[meshIndex].elementID;

					// add faces, verticies, UVS and Normals
					int triangle_num = 0;
					for (int i = 0; i < 6; i++)
					{
						int newIndex = index + bMask[i].X + (bMask[i].Y * chunkLineElementsExt) + (bMask[i].Z * chunkLineElementsP2Ext);

						bool flag = false;
						if (newIndex < chunkFields.Num() && newIndex >= 0)
							if (chunkFields[newIndex] < 10) flag = true;	// if see through or none


						if (flag)
						{
							Triangles.Add(bTriangles[0] + triangle_num + elementID);
							Triangles.Add(bTriangles[1] + triangle_num + elementID);
							Triangles.Add(bTriangles[2] + triangle_num + elementID);
							Triangles.Add(bTriangles[3] + triangle_num + elementID);
							Triangles.Add(bTriangles[4] + triangle_num + elementID);
							Triangles.Add(bTriangles[5] + triangle_num + elementID);
							triangle_num += 4;	// add 4 verticies to next triangle

							switch (i)
							{
							case 0: {
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals0, ARRAY_COUNT(bNormals0));
								break;
							}
							case 1: {
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals1, ARRAY_COUNT(bNormals1));
								break;
							}
							case 2: {
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));

								Normals.Append(bNormals2, ARRAY_COUNT(bNormals2));
								break;
							}
							case 3: {
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Normals.Append(bNormals3, ARRAY_COUNT(bNormals3));
								break;
							}
							case 4: {
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Normals.Append(bNormals4, ARRAY_COUNT(bNormals4));
								break;
							}
							case 5: {
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), -voxelSizeHalf + (z * voxelSize)));
								Vertices.Add(FVector(-voxelSizeHalf + (x * voxelSize), -voxelSizeHalf + (y * voxelSize), voxelSizeHalf + (z * voxelSize)));
								Normals.Append(bNormals5, ARRAY_COUNT(bNormals5));
								break;
							}
							}

							UVs.Append(bUVs, ARRAY_COUNT(bUVs));
							//FColor color = FColor::MakeRandomColor();
							//FColor color = FColor(RandomStream.FRand() * 256, RandomStream.FRand() * 256, RandomStream.FRand() * 256, rand() % 5);
							FColor color = FColor(255, 255, 255, i);
							VertexColors.Add(color); VertexColors.Add(color); VertexColors.Add(color); VertexColors.Add(color);
						}
					}
					
					el_num += triangle_num;
					meshSections[meshIndex].elementID += triangle_num;
				}
			}
		}
	}
	
	proceduralComponent->ClearAllMeshSections();
	for (int i = 0; i < meshSections.Num(); i++)
	{
		if (meshSections[i].Vertices.Num() > 0)
			proceduralComponent->CreateMeshSection(i, meshSections[i].Vertices, meshSections[i].Triangles, meshSections[i].Normals, meshSections[i].UVs, meshSections[i].VertexColors, meshSections[i].Tangents, bCreateCollision);
	}

	ApplyMaterials();
}

void AVoxelActor::ApplyMaterials()
{
	int s = 0;
	if (voxelAdministration->settingMaterial)
		while (s < Materials.Num())
		{
			proceduralComponent->SetMaterial(s, Materials[s]);
			s++;
		}
	else
		while (s < MaterialsLow.Num())
		{
			proceduralComponent->SetMaterial(s, MaterialsLow[s]);
			s++;
		}
}

void AVoxelActor::UpdateExtras()
{
	if (voxelAdministration->settingObjectsStatic || voxelAdministration->settingObjectsDynamic)
		for (int32 x = 0; x < chunkLineElements; x++)
		{
			for (int32 y = 0; y < chunkLineElements; y++)
			{
				for (int32 z = 0; z < chunkZElements; z++)
				{
					int32 index = (x + 1) + (chunkLineElementsExt * (y + 1)) + (chunkLineElementsP2Ext * z);
					int32 meshIndex = chunkFields[index];

					if (meshIndex < 0 && meshIndex >= -7 && voxelAdministration->settingObjectsStatic)
						AddInstanceVoxel(FVector(x * voxelSize, y * voxelSize, z * voxelSize), abs(meshIndex)-1);
					else if (meshIndex < -7 && voxelAdministration->settingObjectsDynamic)
						AddInstanceVoxel(FVector(x * voxelSize, y * voxelSize, z * voxelSize), abs(meshIndex) - 1);
				}
			}
		}
}

TArray<int32> AVoxelActor::calculateNoise()
{
	TArray<int32> noises;
	noises.Reserve(chunkLineElementsExt * chunkLineElementsExt);
	for (int32 y = -1; y <= chunkLineElements; y++)
	{
		for (int32 x = -1; x <= chunkLineElements; x++)
		{
	
			float noiseValue = 
			USimplexNoiseLibrary::SimplexNoise2D((chunkXindex*chunkLineElements + x) * 0.01f, (chunkYindex*chunkLineElements + y) * 0.01f) *  4+ 
			USimplexNoiseLibrary::SimplexNoise2D((chunkXindex*chunkLineElements + x) * 0.01f, (chunkYindex*chunkLineElements + y) * 0.01f) * 8 +
			USimplexNoiseLibrary::SimplexNoise2D((chunkXindex*chunkLineElements + x) * 0.004f, (chunkYindex*chunkLineElements + y) * 0.004f) * firstmultiplier +
			FMath::Clamp(USimplexNoiseLibrary::SimplexNoise2D((chunkXindex*chunkLineElements + x) * 0.05f, (chunkYindex*chunkLineElements + y) * 0.05f), 0.0f, 5.0f) * 4; // clamp 0-5
			noises.Add(FMath::FloorToInt(noiseValue));
		}
	}
	return noises;
}

void AVoxelActor::setVoxel(FVector localPos, int32 value)
{
	int32 x = localPos.X / voxelSize + 1;
	int32 y = localPos.Y / voxelSize + 1;
	int32 z = localPos.Z / voxelSize;

	int32 index = x + (y * chunkLineElementsExt) + (z * chunkLineElementsP2Ext);

	UE_LOG(VoxelLog, Log, TEXT("VALUE = %d"), chunkFields[index]);

	chunkFields[index] = value;
	
	FVoxel fv = FVoxel();
	fv.Index = index;
	fv.Value = value;
	voxelAdministration->UpdateVoxel(chunkXindex, chunkYindex, fv); // updating voxel data
	UpdateMesh();
}

void AVoxelActor::getVoxel(FVector localPos, int32 &voxelType, int32 &index)
{
	int32 x = localPos.X / voxelSize + 1;
	int32 y = localPos.Y / voxelSize + 1;
	int32 z = localPos.Z / voxelSize;

	index = x + (y * chunkLineElementsExt) + (z * chunkLineElementsP2Ext);
	voxelType = chunkFields[index];
}

bool AVoxelActor::inRange(int32 value, int32 range)
{
	return (value >= 0 && value <= range);
}

void AVoxelActor::updateCollisionRange(bool isCollisionRange)
{
	if (bCreateCollision != isCollisionRange)
	{
		bCreateCollision = isCollisionRange;
		UpdateMesh();
	}
}

void AVoxelActor::AddInstanceVoxel_Implementation(FVector instanceLocation, int32 type)
{
	
}
