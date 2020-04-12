// Copyright 2016-2017 Tefel. All Rights Reserved.

#pragma once

#include "Engine/GameInstance.h"
#include "VoxelAdministration.h"
#include "VoxelGameInstance.generated.h"

/**
*	Save (name + time)
*/
USTRUCT(BlueprintType)
struct FSave
{
	GENERATED_USTRUCT_BODY()
	FSave(FString name = "", FDateTime date = FDateTime()) : saveName(name), dateTime(date) {}

	UPROPERTY(BlueprintReadWrite)
	FString saveName;

	UPROPERTY(BlueprintReadWrite)
	FDateTime dateTime;

};

/**
 * Game instance supporting savegame and setting loading screen from Slate widget
 */
UCLASS()
class UNREALVOXEL_API UVoxelGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	
	// if empty "" means it's a new world
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString savegameName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 randomSeed = 0;

	// added to noise multipliers
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float firstmultiplier = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float noisemultiplier2 = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float noisemultiplier3 = 0.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float noisemultiplier4 = 0.0;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString randomSeedName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<int32> customSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AVoxelAdministration * voxelAdministration;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 loadingProgress = 0;

	UPROPERTY()
	UTexture * texture;

	UFUNCTION(BlueprintCallable, Category = TDLHelpers)
	static TArray<FSave> GetAllSaveGameSlotNames();

	UFUNCTION()
	virtual void BeginLoadingScreen(const FString & name);

	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* LoadedWorld);

};
