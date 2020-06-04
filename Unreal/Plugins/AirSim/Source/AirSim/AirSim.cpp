// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "AirSim.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
#include "AirSimPluginSettings.h"
#include "Developer/Settings/Public/ISettingsModule.h"


IMPLEMENT_MODULE( FAirSim, AirSim )

DEFINE_LOG_CATEGORY(LogAirSim)

#define LOCTEXT_NAMESPACE "AirSim"

void FAirSim::StartupModule()
{
    //plugin startup
    UE_LOG(LogAirSim, Log, TEXT("StartupModule: AirSim plugin"));

	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FAirSim::OnPostEngineInit);
}

void FAirSim::ShutdownModule()
{
	FCoreDelegates::OnPostEngineInit.RemoveAll(this);
    //plugin shutdown

	if (UObjectInitialized())
	{
		UnregisterSettings();
	}
}

void FAirSim::OnPostEngineInit()
{
	RegisterSettings();
}

void FAirSim::RegisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->RegisterSettings("Project", "Plugins", "AirSim",
			LOCTEXT("RuntimeSettingsName", "AirSim"),
			LOCTEXT("RuntimeSettingsDescription", "Configure the AirSim plugin"),
			GetMutableDefault<UAirSimPluginSettings>());
	}
}

void FAirSim::UnregisterSettings()
{
	if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
	{
		SettingsModule->UnregisterSettings("Project", "Plugins", "AirSim");
	}
}

#undef LOCTEXT_NAMESPACE