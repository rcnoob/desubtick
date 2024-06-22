#include "desubtick.h"
#include <fstream>
#include <iostream>

#include "utils/addresses.h"

#include "tier0/memdbgon.h"
#include "utils/cdetour.h"

DesubtickPlugin g_DesubtickPlugin;
CUtlVector<CDetourBase*> g_vecDetours;
float desiredTickrate = 128.0f;

#define DECLARE_BASEPLAYER_DETOUR(name) DECLARE_DETOUR(name, BasePlayer::Detour_##name, &modules::server);
#define DECLARE_BASEPLAYER_EXTERN_DETOUR(name) extern CDetour<decltype(BasePlayer::Detour_##name)> name;

namespace BasePlayer
{
	void FASTCALL Detour_ProcessUsercmds(void* self, CUserCmd* usercmd, int totalcmds, bool paused);
}

DECLARE_BASEPLAYER_EXTERN_DETOUR(ProcessUsercmds);
DECLARE_BASEPLAYER_DETOUR(ProcessUsercmds);



PLUGIN_EXPOSE(DesubtickPlugin, g_DesubtickPlugin);

bool DesubtickPlugin::Load(PluginId id, ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	PLUGIN_SAVEVARS();
	modules::server = new CModule(GAMEBIN, "server");
	std::ifstream configFile("tickrate.txt");

    if (configFile.is_open()) {
        configFile >> desiredTickrate;
        configFile.close();
    }
	INIT_DETOUR(ProcessUsercmds);

	return true;
}

void FASTCALL BasePlayer::Detour_ProcessUsercmds(void* self, CUserCmd* usercmd, int totalcmds, bool paused)
{
	float subticksPerTick = desiredTickrate / 64.0f;
	float accumulatedSubticks = 0.0f;
	for(int x = 0; x < totalcmds; x++)
	{
		CUserCmd* ptr = reinterpret_cast<CUserCmd*>(reinterpret_cast<char*>(usercmd) + (sizeof(CUserCmd)*x));
		int subtick_moves_count = ptr->base->subtick_moves.current_size;

		for(int i = 0; i < subtick_moves_count; i++)
		{
			float when = accumulatedSubticks - static_cast<int>(accumulatedSubticks);
			ptr->base->subtick_moves.rep->elements[i]->when = when;

			accumulatedSubticks += subticksPerTick;
		}
	}
	ProcessUsercmds(self, usercmd, totalcmds, paused);
}

bool DesubtickPlugin::Unload(char *error, size_t maxlen)
{	
	return true;
}

void DesubtickPlugin::AllPluginsLoaded()
{
}

bool DesubtickPlugin::Pause(char *error, size_t maxlen)
{
	return true;
}

bool DesubtickPlugin::Unpause(char *error, size_t maxlen)
{
	return true;
}

const char *DesubtickPlugin::GetLicense()
{
	return "GPLv3";
}

const char *DesubtickPlugin::GetVersion()
{
	return "1.0.0";
}

const char *DesubtickPlugin::GetDate()
{
	return __DATE__;
}

const char *DesubtickPlugin::GetLogTag()
{
	return "desubtick";
}

const char *DesubtickPlugin::GetAuthor()
{
	return "carnifex";
}

const char *DesubtickPlugin::GetDescription()
{
	return "Disables subticked movement";
}

const char *DesubtickPlugin::GetName()
{
	return "CS2 Desubtick";
}

const char *DesubtickPlugin::GetURL()
{
	return "";
}
