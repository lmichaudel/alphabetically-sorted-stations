#pragma once
#include "CoreMinimal.h"
#include "Configuration/ConfigManager.h"
#include "Engine/Engine.h"
#include "AlphabeticallySortedStationsModule_ConfigStruct.generated.h"

struct FAlphabeticallySortedStationsModule_ConfigStruct_Map;
struct FAlphabeticallySortedStationsModule_ConfigStruct_Trains;
struct FAlphabeticallySortedStationsModule_ConfigStruct_Drones;

USTRUCT(BlueprintType)
struct FAlphabeticallySortedStationsModule_ConfigStruct_Map {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    int32 map_sort_train_stations{};

    UPROPERTY(BlueprintReadWrite)
    int32 map_sort_trains{};

    UPROPERTY(BlueprintReadWrite)
    int32 map_sort_drone_stations{};

    UPROPERTY(BlueprintReadWrite)
    int32 map_sort_drones{};

    UPROPERTY(BlueprintReadWrite)
    int32 map_sort_vehicle_stations{};
};

USTRUCT(BlueprintType)
struct FAlphabeticallySortedStationsModule_ConfigStruct_Trains {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    int32 trains_sort_trains{};

    UPROPERTY(BlueprintReadWrite)
    int32 trains_sort_train_stations{};
};

USTRUCT(BlueprintType)
struct FAlphabeticallySortedStationsModule_ConfigStruct_Drones {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    int32 drones_sort_drone_stations{};
};

/* Struct generated from Mod Configuration Asset '/AlphabeticallySortedStations/AlphabeticallySortedStationsModule_Config' */
USTRUCT(BlueprintType)
struct FAlphabeticallySortedStationsModule_ConfigStruct {
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite)
    FAlphabeticallySortedStationsModule_ConfigStruct_Map Map{};

    UPROPERTY(BlueprintReadWrite)
    FAlphabeticallySortedStationsModule_ConfigStruct_Trains Trains{};

    UPROPERTY(BlueprintReadWrite)
    FAlphabeticallySortedStationsModule_ConfigStruct_Drones Drones{};

    /* Retrieves active configuration value and returns object of this struct containing it */
    static FAlphabeticallySortedStationsModule_ConfigStruct GetActiveConfig(UObject* WorldContext) {
        FAlphabeticallySortedStationsModule_ConfigStruct ConfigStruct{};
        FConfigId ConfigId{"AlphabeticallySortedStations", ""};
        if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::ReturnNull)) {
            UConfigManager* ConfigManager = World->GetGameInstance()->GetSubsystem<UConfigManager>();
            ConfigManager->FillConfigurationStruct(ConfigId, FDynamicStructInfo{FAlphabeticallySortedStationsModule_ConfigStruct::StaticStruct(), &ConfigStruct});
        }
        return ConfigStruct;
    }
};

