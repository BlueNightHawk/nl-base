#include "hud.h"
#include "cl_util.h"
#include "filesystem_utils.h"

#include "nlfuncs.h"
#include "PlatformHeaders.h"

#include <filesystem>

#include "registry.h"

#define FS_FILENAME_MAX 128

char s_pBaseDir[512];

void COM_FixSlashes(char* pname);
/*
{
	for (char* pszNext = pname; *pszNext; ++pszNext)
	{
		if (*pszNext == '\\')
			*pszNext = '/';
	}
}
*/

FileHandle_t FS_Open(const char* pFileName, const char* pOptions)
{
	return g_pFileSystem->Open(pFileName, pOptions);
}

FileHandle_t FS_OpenPathID(const char* pFileName, const char* pOptions, const char* pathID)
{
	return g_pFileSystem->Open(pFileName, pOptions, pathID);
}

void FS_Close(FileHandle_t file)
{
	g_pFileSystem->Close(file);
}

bool FS_EndOfFile(FileHandle_t file)
{
	return g_pFileSystem->EndOfFile(file);
}

char* FS_ReadLine(char* pOutput, int maxChars, FileHandle_t file)
{
	return g_pFileSystem->ReadLine(pOutput, maxChars, file);
}

void FS_AddSearchPath(const char* pPath, const char* pathID)
{
	g_pFileSystem->AddSearchPath(pPath, pathID);
}

void FileSystem_AddDirectory(bool bReadOnly, const char* pszPathID, const char* pszFormat, ...)
{
	char temp[FS_FILENAME_MAX];

	va_list list;

	va_start(list, pszFormat);
	vsnprintf(temp, sizeof(temp) - 1, pszFormat, list);
	va_end(list);

	temp[sizeof(temp) - 1] = '\0';
	COM_FixSlashes(temp);

	if (bReadOnly)
		g_pFileSystem->AddSearchPathNoWrite(temp, pszPathID);
	else
		g_pFileSystem->AddSearchPath(temp, pszPathID);
}

bool BEnableAddonsFolder()
{
	if (gEngfuncs.CheckParm("-addons",0) != 0)
		return true;

	return registry->ReadInt("addons_folder", 0) > 0;
}

bool BEnabledHDAddon()
{
	if (gEngfuncs.CheckParm("-nohdmodels", 0) != 0)
		return false;

	return registry->ReadInt("hdmodels", 0) > 0;
}

bool FileSystem_AddFallbackGameDir(const char* pGameDir)
{
	char language[FS_FILENAME_MAX];

	language[0] = '\0';

	{
//		const char* pszCurrentLanguage;

//		if (SteamApps() && ((pszCurrentLanguage = SteamApps()->GetCurrentGameLanguage()) != nullptr))
//		{
//			strncpy(language, pszCurrentLanguage, sizeof(language));
//		}
//		else
		{
			strncpy(language, "english", sizeof(language));
		}
	}

	if (language[0] && stricmp(language, "english"))
	{
		char temp[4096];
		snprintf(temp, sizeof(temp), "%s/%s_%s", s_pBaseDir, pGameDir, language);
		g_pFileSystem->AddSearchPath(temp, "GAME");
	}

	g_pFileSystem->AddSearchPath(pGameDir, "GAME");

	return true;
}

void CheckLiblistForFallbackDir(const char* pGameDir, bool bLanguage, const char* pLanguage, bool bLowViolenceBuild)
{
	char szTemp[512];

	snprintf(szTemp, sizeof(szTemp) - 1, "%s/liblist.gam", pGameDir);
	COM_FixSlashes(szTemp);
	g_pFileSystem->GetLocalCopy(szTemp);

	FileHandle_t hFile;

	if (!stricmp(gEngfuncs.pfnGetGameDirectory(), pGameDir))
	{
		hFile = FS_Open("liblist.gam", "rt");
	}
	else
	{
		snprintf(szTemp, sizeof(szTemp) - 1, "../%s/liblist.gam", pGameDir);
		COM_FixSlashes(szTemp);
		hFile = FS_Open(szTemp, "rt");
	}

	if (hFile == FILESYSTEM_INVALID_HANDLE)
		return;

	char szLine[512];

	const char* pszDir;

	size_t uiLength;

	bool bFound = false;

	while (1)
	{
		if (FS_EndOfFile(hFile))
			break;

		szLine[0] = '\0';
		FS_ReadLine(szLine, sizeof(szLine) - 1, hFile);
		szLine[sizeof(szLine) - 1] = '\0';

		if (!strnicmp(szLine, "fallback_dir", 12))
		{
			const char* pszStartQuote = strchr(szLine, '"');

			if (!pszStartQuote)
				break;

			pszDir = pszStartQuote + 1;

			const char* pszEndQuote = strchr(pszStartQuote + 1, '"');

			if (!pszEndQuote)
				break;

			uiLength = pszEndQuote - pszStartQuote - 1;

			if (uiLength >= (FS_FILENAME_MAX - 1))
				break;

			if (uiLength)
			{
				char szFallback[FS_FILENAME_MAX];
				strncpy(szFallback, pszDir, uiLength);
				szFallback[uiLength] = '\0';

				if (stricmp(pGameDir, szFallback))
				{
					if (bLowViolenceBuild)
					{
						FileSystem_AddDirectory(true, "GAME_FALLBACK", "%s/%s_lv", s_pBaseDir, szFallback);
					}

					if (BEnableAddonsFolder())
					{
						FileSystem_AddDirectory(true, "GAME_FALLBACK", "%s/%s_addon", s_pBaseDir, szFallback);
					}

					if (bLanguage)
					{
						if (pLanguage)
						{
							FileSystem_AddDirectory(false, "GAME_FALLBACK", "%s/%s_%s", s_pBaseDir, szFallback, pLanguage);
							if (!gEngfuncs.CheckParm("-steam", 0))
							{
								char baseDir[4096];
								strncpy(baseDir, s_pBaseDir, sizeof(baseDir));
								baseDir[sizeof(baseDir) - 1] = '\0';

								char* pszGamePath = strstr(baseDir, "\\game");

								if (pszGamePath)
								{
									*pszGamePath = '\0';
									FileSystem_AddDirectory(false, "GAME_FALLBACK", "%s\\localization\\%s_%s", baseDir, szFallback, pLanguage);
								}
							}
						}
					}

					if (BEnabledHDAddon())
					{
						FileSystem_AddDirectory(true, "GAME_FALLBACK", "%s/%s_hd", s_pBaseDir, szFallback);
					}

					FileSystem_AddDirectory(false, "GAME_FALLBACK", "%s/%s", s_pBaseDir, szFallback);

					if (strcmp("valve", szFallback))
					{
						char* pszLangFilename = new char[FS_FILENAME_MAX];
						snprintf(pszLangFilename, FS_FILENAME_MAX - 1, "Resource/%s_%%language%%.txt", szFallback);
						pszLangFilename[FS_FILENAME_MAX - 1] = '\0';
						// Recursively include fallback directories for the games that are being used as a fallback.
						// TODO: can result in infinite recursion.
						CheckLiblistForFallbackDir(szFallback, bLanguage, pLanguage, bLowViolenceBuild);
					}
				}
			}
		}
	}
	FS_Close(hFile);
}

bool FileSystem_SetGameDirectory(const char* pDefaultDir, const char* pGameDir)
{
	char language[FS_FILENAME_MAX];
	language[0] = '\0';

	{
	//	const char* pszCurrentLanguage;

	//	if (SteamApps() && ((pszCurrentLanguage = SteamApps()->GetCurrentGameLanguage()) != 0))
	//	{
	//		strncpy(language, pszCurrentLanguage, sizeof(language));
	//	}
	//	else
		{
			strncpy(language, "english", sizeof(language));
		}
	}

	// SteamAPI_SetBreakpadAppID( GetGameAppID() );

	const bool bLanguage = language[0] && stricmp(language, "english") != 0;

	char baseDir[4096];

	// Mod directories, if specified.
	if (pGameDir || pDefaultDir)
	{
		// Mod dir can be null if this is the default directory ("valve" in GoldSource)
		const char* const pszModDir = pGameDir ? pGameDir : pDefaultDir;

		if (0)
		{
			FileSystem_AddDirectory(true, "GAME", "%s/%s_lv", s_pBaseDir, pszModDir);
		}

		if (BEnableAddonsFolder())
		{
			FileSystem_AddDirectory(true, "GAME", "%s/%s_addon", s_pBaseDir, pszModDir);
		}

		if (bLanguage)
		{
			FileSystem_AddDirectory(true, "GAME", "%s/%s_%s", s_pBaseDir, pszModDir, language);

			if (!gEngfuncs.CheckParm("-steam",0))
			{
				strncpy(baseDir, s_pBaseDir, sizeof(baseDir));
				baseDir[sizeof(baseDir) - 1] = '\0';

				char* pszGamePath = strstr(baseDir, "\\game");

				if (pszGamePath)
				{
					*pszGamePath = '\0';
					FileSystem_AddDirectory(true, "GAME", "%s\\localization\\%s_%s", baseDir, pszModDir, language);
				}
			}
		}
		if (BEnabledHDAddon())
		{
			FileSystem_AddDirectory(true, "GAME", "%s/%s_hd", s_pBaseDir, pszModDir);
		}

		FileSystem_AddDirectory(false, "GAME", "%s/%s", s_pBaseDir, pszModDir);
		FileSystem_AddDirectory(false, "GAMECONFIG", "%s/%s", s_pBaseDir, pszModDir);
		FileSystem_AddDirectory(false, "GAMEDOWNLOAD", "%s/%s_downloads", s_pBaseDir, pszModDir);

		CheckLiblistForFallbackDir(pszModDir, bLanguage, language, false);
	}

	// Default directories
	if (bLanguage)
	{
		if (0)
		{
			FileSystem_AddDirectory(true, "DEFAULTGAME", "%s/%s_lv", s_pBaseDir, pDefaultDir);
		}

		if (gEngfuncs.CheckParm("-addons", 0) || registry->ReadInt("addons_folder", 0) > 0)
		{
			FileSystem_AddDirectory(true, "DEFAULTGAME", "%s/%s_addon", s_pBaseDir, pDefaultDir);
		}

		FileSystem_AddDirectory(true, "DEFAULTGAME", "%s/%s_%s", s_pBaseDir, pDefaultDir, language);

		if (gEngfuncs.CheckParm("-steam",0))
		{
			strncpy(baseDir, s_pBaseDir, sizeof(baseDir));
			baseDir[sizeof(baseDir) - 1] = '\0';

			char* pszGamePath = strstr(baseDir, "\\game");

			if (pszGamePath)
			{
				*pszGamePath = '\0';
				FileSystem_AddDirectory(true, "DEFAULTGAME", "%s\\localization\\%s_%s", baseDir, pDefaultDir, language);
			}
		}
	}

	if (BEnabledHDAddon())
	{
		FileSystem_AddDirectory(true, "DEFAULTGAME", "%s/%s_hd", s_pBaseDir, pDefaultDir);
	}

	FileSystem_AddDirectory(false, "BASE", "%s", s_pBaseDir);
	FileSystem_AddDirectory(true, "DEFAULTGAME", "%s/%s", s_pBaseDir, pDefaultDir);
	FileSystem_AddDirectory(false, "PLATFORM", "%s/platform", s_pBaseDir);
	return true;
}

void COM_AddAppDirectory(const char* pszBaseDir)
{
	FS_AddSearchPath(pszBaseDir, "PLATFORM");
}


void SetupPaths()
{
	g_pFileSystem->RemoveAllSearchPaths();
	sprintf(s_pBaseDir, "%s/", std::filesystem::current_path().string().c_str());

	g_pFileSystem->AddSearchPath(s_pBaseDir, "ROOT");

	FileSystem_SetGameDirectory("valve", gEngfuncs.pfnGetGameDirectory());

	FileSystem_AddFallbackGameDir(gEngfuncs.pfnGetGameDirectory());
	FileSystem_AddFallbackGameDir("valve");
	COM_AddAppDirectory(s_pBaseDir);
}