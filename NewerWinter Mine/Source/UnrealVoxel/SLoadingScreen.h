// Copyright 2016-2017 Tefel. All Rights Reserved.

#pragma once
#include "SlateBasics.h"
 
class SLoadingScreen : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SLoadingScreen){}
 
	SLATE_ARGUMENT(TWeakObjectPtr<class UVoxelGameInstance>, OwnerGameInstance)
 
	SLATE_END_ARGS()
 
public:
	void Construct(const FArguments& InArgs);
 
private:
	TWeakObjectPtr<class UVoxelGameInstance> OwnerGameInstance;

};