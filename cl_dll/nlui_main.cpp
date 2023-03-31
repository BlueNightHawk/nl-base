#include "PlatformHeaders.h"
#include "hud.h"
#include "cl_util.h"
#include "nlui_main.h"
#include "nlui_chapterselect.h"
#include <algorithm>
#include <cmath>

CImGuiManager g_ImGuiManager;

void CImGuiManager::Init()
{
	ParseLocaleFile();

	g_ChapterSelect.Init();
	m_SavedStyle = ImGui::GetStyle();
}

void CImGuiManager::Shutdown()
{
	for (size_t i = 0; i < m_pImguiWindows.size(); i++)
	{
		m_pImguiWindows.at(i)->Shutdown();
	}
}

void CImGuiManager::Update()
{
	for (size_t i = 0; i < m_pImguiWindows.size(); i++)
	{
		m_pImguiWindows.at(i)->Update();
	}

	float scale = NLUtils::ScaleFont();

	ImGuiIO& io = ImGui::GetIO();
	io.FontGlobalScale = std::clamp(scale * 0.7f, 0.7f, 1.8f);

	ImGui::GetStyle() = m_SavedStyle;
	ImGui::GetStyle().ScaleAllSizes(std::clamp(scale, 1.0f, 2.5f));

	m_flTime = gEngfuncs.GetAbsoluteTime();
	if (m_flTime != m_flOldTime)
	{
		m_flTimeDelta = fabs(m_flTime - m_flOldTime);
		m_flOldTime = m_flTime;
	}
}

void CImGuiManager::AddWindow(CImGuiWindow* pWindow)
{
	m_pImguiWindows.push_back(pWindow);
}


string CImGuiManager::GetStringLocale(string str)
{
	auto p = m_LocaleList.find(str);
	if (p != m_LocaleList.end())
	{
		return p->second;
	}
	return str;
}

void CImGuiManager::ParseLocaleFile()
{
	static char token[512], token2[512];
	char *pfile, *afile;
	pfile = afile = reinterpret_cast<char*>(gEngfuncs.COM_LoadFile("resource/nlui/english.txt", 5, 0));

	if (!pfile || !afile)
		return;

	while (pfile = gEngfuncs.COM_ParseFile(pfile, token))
	{
		pfile = gEngfuncs.COM_ParseFile(pfile, token2);
		m_LocaleList.insert(std::make_pair(token, token2));
	}

	gEngfuncs.COM_FreeFile(afile);
}