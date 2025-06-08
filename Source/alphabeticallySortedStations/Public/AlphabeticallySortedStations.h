// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AlphabeticallySortedStationsModule_ConfigStruct.h"


DECLARE_LOG_CATEGORY_EXTERN(LogAlphabeticallySortedStations, Verbose, All);

class FAlphabeticallySortedStationsModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
