#ifndef COMMON_GAMEUI_IGAMEUI_H
#define COMMON_GAMEUI_IGAMEUI_H

#include "interface.h"

class IBaseSystem;

typedef struct cl_enginefuncs_s cl_enginefunc_t;

// reasons why the user can't connect to a game server
enum ESteamLoginFailure
{
	STEAMLOGINFAILURE_NONE,
	STEAMLOGINFAILURE_BADTICKET,
	STEAMLOGINFAILURE_NOSTEAMLOGIN,
	STEAMLOGINFAILURE_VACBANNED,
	STEAMLOGINFAILURE_LOGGED_IN_ELSEWHERE,
	STEAMLOGINFAILURE_CONNECTIONLOST,
	STEAMLOGINFAILURE_NOCONNECTION
};

/**
 *	Interface to the GameUI library.
 */
class IGameUI : public IBaseInterface
{
public:
	/**
	 *	Called when this interface is first loaded. Initializes the game UI.
	 *	@param factories Array of factories to use
	 *	@param count Number of factories
	 */
	virtual void Initialize(CreateInterfaceFn* factories, int count) = 0;

	/**
	 *	Sets up the game UI.
	 */
	virtual void Start(cl_enginefunc_t* engineFuncs, int interfaceVersion, IBaseSystem* system) = 0;

	/**
	 *	Shuts down the UI.
	 */
	virtual void Shutdown() = 0;

	/**
	 *	Activates the Game UI, and pauses singleplayer games
	 */
	virtual int ActivateGameUI() = 0;

	/**
	 *	Opens the Demo dialog.
	 */
	virtual int ActivateDemoUI() = 0;

	/**
	 *	@return Whether the Game UI has exclusive input at this time
	 */
	virtual int HasExclusiveInput() = 0;

	/**
	 *	Should be called every frame
	 */
	virtual void RunFrame() = 0;

	/**
	 *	Call when connecting to a server
	 *	@param game Name of the game. This is the mod directory name
	 */
	virtual void ConnectToServer(const char* game, int IP, int port) = 0;

	/**
	 *	Call when disconnecting from a server
	 */
	virtual void DisconnectFromServer() = 0;

	/**
	 *	Hides the Game UI if visible, and unpauses singleplayer games
	 */
	virtual void HideGameUI() = 0;

	/**
	 *	@return Whether the Game UI is visible
	 */
	virtual int IsGameUIActive() = 0;

	/**
	 *	Call when a resource (e.g. "level") has started loading
	 */
	virtual void LoadingStarted(const char* resourceType, const char* resourceName) = 0;

	/**
	 *	Call when a resource (e.g. "level") has finished loading
	 */
	virtual void LoadingFinished(const char* resourceType, const char* resourceName) = 0;

	/**
	 *	Start the progress bar for an event
	 *	@param progresType Type of progress
	 *	@param progressSteps Number of steps in this event
	 */
	virtual void StartProgressBar(const char* progressType, int progressSteps) = 0;

	/**
	 *	Continues the progress bar
	 *	@param progressPoint Progress point that has been reached
	 *	@param progressFraction Fraction of point progress
	 *	@return Whether the loading dialog is visible
	 */
	virtual int ContinueProgressBar(int progressPoint, float progressFraction) = 0;

	/**
	 *	Stops the progress bar
	 *	@param bError Whether an error occurred to stop progress
	 *	@param failureReason If this is an error, displays this as the reason
	 *	@param extendedReason If this is an error, displays this as the description for the reason
	 */
	virtual void StopProgressBar(bool bError, const char* failureReason, const char* extendedReason) = 0;

	/**
	 *	Sets the progress bar status text
	 *	@param statusText Text to set
	 *	@return Whether the text could be set
	 */
	virtual int SetProgressBarStatusText(const char* statusText) = 0;

	/**
	 *	Sets the secondary progress bar's progress
	 */
	virtual void SetSecondaryProgressBar(float progress) = 0;

	/**
	 *	Sets the secondary progress bar's text
	 */
	virtual void SetSecondaryProgressBarText(const char* statusText) = 0;

	/**
	 *	Obsolete, does nothing
	 */
	virtual void ValidateCDKey(bool force, bool inConnect) = 0;

	/**
	 *	Call when the client has disconnected due to a Steam login failure
	 *	@param eSteamLoginFailure Steam error code
	 *	@param username Client username to display
	 */
	virtual void OnDisconnectFromServer(int eSteamLoginFailure, const char* username) = 0;
};

#define GAMEUI_INTERFACE_VERSION "GameUI007"

void InitGameUI();
void ShutdownGameUI();

extern IGameUI* staticGameUIFuncs;

#endif // COMMON_GAMEUI_IGAMEUI_H
