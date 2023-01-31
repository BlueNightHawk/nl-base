#include "cl_dll.h"
#include "cdll_int.h"
#include <memory>
#include <time.h>
#include "discord_rpc.h"
#include "nlfuncs.h"

#define RPC_ID "1048930341039112222"

static int64_t g_iSavedTime = 0;

void InitDiscord()
{
	DiscordEventHandlers handlers;
	memset(&handlers, 0, sizeof(handlers));
#if 0
	handlers.ready = handleDiscordReady;
	handlers.errored = handleDiscordError;
	handlers.disconnected = handleDiscordDisconnected;
	handlers.joinGame = handleDiscordJoinGame;
	handlers.spectateGame = handleDiscordSpectateGame;
	handlers.joinRequest = handleDiscordJoinRequest;
#endif
	// Discord_Initialize(const char* applicationId, DiscordEventHandlers* handlers, int autoRegister, const char* optionalSteamId)
	Discord_Initialize(RPC_ID, &handlers, 1, nullptr);

	g_iSavedTime = time(0);
}

void UpdatePresence()
{
	char buffer[256];
	DiscordRichPresence discordPresence;
	memset(&discordPresence, 0, sizeof(discordPresence));
	discordPresence.largeImageKey = "default";
	discordPresence.startTimestamp = g_iSavedTime;

	if (gEngfuncs.GetEntityByIndex(0) == nullptr)
		discordPresence.details = "Main Menu";
	else if (g_refparams.paused != 0)
	{
		discordPresence.details = "Paused";
	}
	else
	{
		discordPresence.details = "Playing";
	}

	if (gEngfuncs.pfnGetLevelName() && strlen(gEngfuncs.pfnGetLevelName()) > 0)
	{
		discordPresence.state = gEngfuncs.pfnGetLevelName() + 5;
	}

	Discord_UpdatePresence(&discordPresence);
}