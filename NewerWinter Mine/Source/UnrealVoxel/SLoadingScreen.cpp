// Copyright 2016-2017 Tefel. All Rights Reserved.

#include "SLoadingScreen.h"
#include "UnrealVoxel.h"
#include "VoxelGameInstance.h"
#include "SThrobber.h"

#define LOCTEXT_NAMESPACE "SLoadingScreen"
 
void SLoadingScreen::Construct(const FArguments& InArgs)
{
	FString ImagePath = TEXT("Content/pictures/Grass.png");
	FName BrushName = FName(*ImagePath);

	ChildSlot
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Center)
			[
				SNew(SThrobber)
			]
			//	+ SVerticalBox::Slot()					// TODO IMAGE
			//.Padding(0, 40)
			//.VAlign(VAlign_Bottom)
			//.HAlign(HAlign_Center)
			//[
			//	SNew(SImage)
			//	.Image(new FSlateImageBrush(BrushName, FVector2D(128, 128)))
			//]
			+ SVerticalBox::Slot()
			.Padding(0,40)
			.VAlign(VAlign_Bottom)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(NSLOCTEXT("MoviePlayerTestLoadingScreen", "LoadingComplete", "Generating world"))
			]
		];
}
 
#undef LOCTEXT_NAMESPACE