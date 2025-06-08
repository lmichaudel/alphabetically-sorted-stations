// Copyright Epic Games, Inc. All Rights Reserved.

#include "AlphabeticallySortedStations.h"
#include "Patching/NativeHookManager.h"


#include "FGRailroadSubsystem.h"
#include "FGTrainStationIdentifier.h"
#include "FGTrain.h"

#include "FGDroneSubsystem.h"
#include "FGDroneStationInfo.h"

#include "FGActorRepresentationManager.h"

#include <algorithm>
#include <functional>

#define LOCTEXT_NAMESPACE "FAlphabeticallySortedStationsModule"

DEFINE_LOG_CATEGORY(LogAlphabeticallySortedStations);


bool CompareAlphabetic(const FString& a, const FString& b) {
	return a.Compare(b) < 0;
}

bool CompareMapMarkers(std::function<bool(const FString&, const FString&)> comparator, const UFGActorRepresentation& a, const UFGActorRepresentation& b) {
	return comparator(a.GetRepresentationText().ToString(), b.GetRepresentationText().ToString());
}

bool CompareTrainStations(std::function<bool(const FString&, const FString&)> comparator, const AFGTrainStationIdentifier* a, const AFGTrainStationIdentifier* b) {
	return comparator(a->GetStationName().ToString(), b->GetStationName().ToString());
}

bool CompareTrains(std::function<bool(const FString&, const FString&)> comparator, const AFGTrain* a, const AFGTrain* b) {
	return comparator(a->GetTrainName().ToString(), b->GetTrainName().ToString());
}

bool CompareDroneStations(std::function<bool(const FString&, const FString&)> comparator, const FDroneStationData a, const FDroneStationData b) {
	return comparator(a.Station.Get()->GetBuildingTag_Implementation(), b.Station.Get()->GetBuildingTag_Implementation());
}

void SortMapMarkers(std::function<bool(const FString&, const FString&)> comparator, TArray<UFGActorRepresentation*>& markers, ERepresentationType type) {
	TArray<UFGActorRepresentation*> Filtered;
	for (UFGActorRepresentation* Marker : markers) {
		if (Marker && Marker->GetRepresentationType() == type) {
			Filtered.Add(Marker);
		}
	}

	Filtered.Sort([comparator](const UFGActorRepresentation& a, const UFGActorRepresentation& b) {
		return CompareMapMarkers(comparator, a, b);
	});

	int32 FilteredIndex = 0;
	for (int32 i = 0; i < markers.Num(); ++i) {
		if (markers[i] && markers[i]->GetRepresentationType() == type) {
			markers[i] = Filtered[FilteredIndex++];
		}
	}
}

void SortTrainStations(std::function<bool(const FString&, const FString&)> comparator, TArray<AFGTrainStationIdentifier*>& stations) {
	std::sort(stations.begin(), stations.end(), [comparator](const auto& a, const auto& b) {
		return CompareTrainStations(comparator, a, b);
	});
}

void SortTrains(std::function<bool(const FString&, const FString&)> comparator, TArray<AFGTrain*>& trains) {
	std::sort(trains.begin(), trains.end(), [comparator](const auto& a, const auto& b) {
		return CompareTrains(comparator, a, b);
	});
}

void SortDroneStations(std::function<bool(const FString&, const FString&)> comparator, TArray<FDroneStationData>& stations) {
	TArray<FDroneStationData> Filtered;
	for (FDroneStationData Station : stations) {
		if (Station.Station) {
			Filtered.Add(Station);
		}
	}

	Filtered.Sort([comparator](const FDroneStationData& a, const FDroneStationData& b) {
		return CompareDroneStations(comparator, a, b);
	});

	int32 FilteredIndex = 0;
	for (int32 i = 0; i < stations.Num(); ++i) {
		if (stations[i].Station) {
			stations[i] = Filtered[FilteredIndex++];
		}
	}
}


void FAlphabeticallySortedStationsModule::StartupModule()
{	
	/////////////////////////////
	// *** SORTING THE MAP *** //
	/////////////////////////////
	SUBSCRIBE_METHOD_AFTER(AFGActorRepresentationManager::AddRepresentation, [](AFGActorRepresentationManager* self, UFGActorRepresentation* actorRepresentation)
		{
			FAlphabeticallySortedStationsModule_ConfigStruct_Map config = FAlphabeticallySortedStationsModule_ConfigStruct::GetActiveConfig(self).Map;

			if(config.map_sort_trains == 0)
				SortMapMarkers(CompareAlphabetic, self->mAllRepresentations, ERepresentationType::RT_Train);
			if (config.map_sort_train_stations == 0)
				SortMapMarkers(CompareAlphabetic, self->mAllRepresentations, ERepresentationType::RT_TrainStation);
			if (config.map_sort_drones == 0)
				SortMapMarkers(CompareAlphabetic, self->mAllRepresentations, ERepresentationType::RT_Drone);
			if (config.map_sort_drone_stations == 0)
				SortMapMarkers(CompareAlphabetic, self->mAllRepresentations, ERepresentationType::RT_DronePort);
			if (config.map_sort_vehicle_stations == 0)
				SortMapMarkers(CompareAlphabetic, self->mAllRepresentations, ERepresentationType::RT_VehicleDockingStation);
		}
	);
	SUBSCRIBE_METHOD_AFTER(AFGActorRepresentationManager::UpdateRepresentation, [](bool r, AFGActorRepresentationManager* self, UFGActorRepresentation* actorRepresentation)
		{
			FAlphabeticallySortedStationsModule_ConfigStruct_Map config = FAlphabeticallySortedStationsModule_ConfigStruct::GetActiveConfig(self).Map;

			if (config.map_sort_trains == 0)
				SortMapMarkers(CompareAlphabetic, self->mAllRepresentations, ERepresentationType::RT_Train);
			if (config.map_sort_train_stations == 0)
				SortMapMarkers(CompareAlphabetic, self->mAllRepresentations, ERepresentationType::RT_TrainStation);
			if (config.map_sort_drones == 0)
				SortMapMarkers(CompareAlphabetic, self->mAllRepresentations, ERepresentationType::RT_Drone);
			if (config.map_sort_drone_stations == 0)
				SortMapMarkers(CompareAlphabetic, self->mAllRepresentations, ERepresentationType::RT_DronePort);
			if (config.map_sort_vehicle_stations == 0)
				SortMapMarkers(CompareAlphabetic, self->mAllRepresentations, ERepresentationType::RT_VehicleDockingStation);
		}
	);
	/////////////////////////////



	/////////////////////////////////////////
	// *** SORTING TRAIN TIME SCHEDULE *** //
	/////////////////////////////////////////
	SUBSCRIBE_METHOD_AFTER(AFGRailroadSubsystem::GetTrainStations, [](const AFGRailroadSubsystem* self, int track_id, TArray<AFGTrainStationIdentifier*>& out_stations)
		{
			FAlphabeticallySortedStationsModule_ConfigStruct_Trains config = FAlphabeticallySortedStationsModule_ConfigStruct::GetActiveConfig(self->GetGameInstance()).Trains;

			if (config.trains_sort_train_stations == 0)
				SortTrainStations(CompareAlphabetic, out_stations);
		}
	);
	SUBSCRIBE_METHOD_AFTER(AFGRailroadSubsystem::GetAllTrainStations, [](const AFGRailroadSubsystem* self, TArray<AFGTrainStationIdentifier*>& out_stations)
		{
			FAlphabeticallySortedStationsModule_ConfigStruct_Trains config = FAlphabeticallySortedStationsModule_ConfigStruct::GetActiveConfig(self->GetGameInstance()).Trains;

			if (config.trains_sort_train_stations == 0)
				SortTrainStations(CompareAlphabetic, out_stations);
		}
	);
	//SUBSCRIBE_METHOD_AFTER(AFGRailroadSubsystem::GetTrains, [](const AFGRailroadSubsystem* self, int32 track_id, TArray<AFGTrain*>& out_trains)
	//	{
	//		FAlphabeticallySortedStationsModule_ConfigStruct_Trains config = FAlphabeticallySortedStationsModule_ConfigStruct::GetActiveConfig(self->GetGameInstance()).Trains;

	//		if (config.trains_sort_trains == 0)
	//			SortTrains(CompareAlphabetic, out_trains);
	//	}
	//);
	//SUBSCRIBE_METHOD_AFTER(AFGRailroadSubsystem::GetAllTrains, [](const AFGRailroadSubsystem* self, TArray<AFGTrain*>& out_trains)
	//	{
	//		FAlphabeticallySortedStationsModule_ConfigStruct_Trains config = FAlphabeticallySortedStationsModule_ConfigStruct::GetActiveConfig(self->GetGameInstance()).Trains;

	//		if (config.trains_sort_trains == 0)
	//			SortTrains(CompareAlphabetic, out_trains);
	//	}
	//);
	/////////////////////////////////////////



	////////////////////////////////////
	// *** SORTING DRONE STATIONS *** //
	////////////////////////////////////
	SUBSCRIBE_METHOD_AFTER(AFGDroneSubsystem::SearchStations, [](AFGDroneSubsystem* self, AFGDroneStationInfo* originStation, AFGDroneStationInfo* hostStation, FString filter, bool connectionsOnly, bool excludeOrigin, bool pairedFirst, bool includeEmptyStation, TArray<FDroneStationData>& result)
		{
			FAlphabeticallySortedStationsModule_ConfigStruct_Drones config = FAlphabeticallySortedStationsModule_ConfigStruct::GetActiveConfig(self->GetGameInstance()).Drones;

			if(config.drones_sort_drone_stations == 0)
				SortDroneStations(CompareAlphabetic, result);
		}
	);
	////////////////////////////////////
}

void FAlphabeticallySortedStationsModule::ShutdownModule() {}


#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FAlphabeticallySortedStationsModule, AlphabeticallySortedStations)