// Copyright 2016-2017 Tefel. All Rights Reserved.

#include "VoxelGameInstance.h"
#include "UnrealVoxel.h"
#include "MoviePlayer.h"
#include "SLoadingScreen.h"
#include "PlatformFeatures.h"
#include "GameFramework/SaveGame.h"
#include "VoxelAdministration.h"
#include "SaveGameSystem.h"

#include "Engine.h"
#include "NoExportTypes.h"

int32 settings[] = { 0, 0, 1, 0, 0, 0,13 };

void UVoxelGameInstance::Init()
{
	UGameInstance::Init();
	customSettings.Append(settings, ARRAY_COUNT(settings));

	for (TActorIterator<AVoxelAdministration> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		voxelAdministration = *ActorItr;
	}
	
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UVoxelGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UVoxelGameInstance::EndLoadingScreen);
}

void UVoxelGameInstance::BeginLoadingScreen(const FString & name)
{
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;

	LoadingScreen.WidgetLoadingScreen = SNew(SLoadingScreen);
	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
}

void UVoxelGameInstance::EndLoadingScreen(UWorld* LoadedWorld)
{
	for (TActorIterator<AVoxelAdministration> ActorItr(LoadedWorld); ActorItr; ++ActorItr)
	{
		voxelAdministration = *ActorItr;
	}
}

TArray<FSave> UVoxelGameInstance::GetAllSaveGameSlotNames()
{
	TArray<FSave> saves;
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();

	FString saveGamePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir()) + "SaveGames/";
	if (!FPaths::DirectoryExists(saveGamePath))
	{
		UE_LOG(VoxelLog, Log, TEXT("Wrong Directory %s"), *saveGamePath);
		TArray<FSave>();
	}
	
	IFileManager& FileManager = IFileManager::Get();
	FString FileExtension = ".sav";
	TArray<FString> SaveGameList;
	FileManager.FindFiles(SaveGameList, *saveGamePath, *FileExtension);

	for (FString SaveGame : SaveGameList)
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
		FFileStatData FileStatData = PlatformFile.GetStatData(*(saveGamePath + "/" + SaveGame));

		if (!SaveGame.Contains("Settings"))
		{
			saves.Add(FSave(SaveGame, FileStatData.CreationTime));
		}
	}

	return saves;
}