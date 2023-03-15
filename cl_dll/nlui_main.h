#pragma once

#include "imgui.h"
#include <vector>
#include <map>
#include <string>
#include "nlui_utils.h"

using std::string;

class CImGuiWindow
{
public:
	virtual void Init() {}
	virtual void Shutdown() {}

	virtual void Update() {}

protected:
	bool m_bOpen = false;
};

class CImGuiManager
{
public:
	void Init();
	void Shutdown();

	void Update();

	void AddWindow(CImGuiWindow *pWindow);

	string GetStringLocale(string str);

private:
	std::vector<CImGuiWindow*> m_pImguiWindows;

	std::map<string, string> m_LocaleList;
	void ParseLocaleFile();

	ImGuiStyle m_SavedStyle;
};

extern CImGuiManager g_ImGuiManager;