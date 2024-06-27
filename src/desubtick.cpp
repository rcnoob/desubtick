#include "desubtick.h"
#include <fstream>
#include <iostream>
#include "networkbasetypes.pb.h"
#include "usercmd.pb.h"
#include "cs_usercmd.pb.h"

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

	INIT_DETOUR(ProcessUsercmds);

	return true;
}

class CUserCmd
{
public:
	CSGOUserCmdPB cmd;
	[[maybe_unused]] char pad1[0x38];
#ifdef PLATFORM_WINDOWS
	[[maybe_unused]] char pad2[0x8];
#endif
};

void FASTCALL BasePlayer::Detour_ProcessUsercmds(CBasePlayerPawn *pPawn, CUserCmd *cmds, int totalcmds, bool paused, float margin)
{
	for (int i = 0; i < totalcmds; i++)
	{
		CSGOUserCmdPB *pUserCmd = &cmds[i].cmd;

		for (int j = 0; j < pUserCmd->mutable_base()->subtick_moves_size(); j++)
			pUserCmd->mutable_base()->mutable_subtick_moves(j)->set_when(0.f);
	}

	ProcessUsercmds(pPawn, cmds, totalcmds, paused, margin);
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
