#include "hud.h"
#include "nlui_main.h"
#include "nlui_chapterselect.h"
#include "igameui.h"

CChapterSelect g_ChapterSelect;

void CChapterSelect::Open()
{
	m_bOpen = true;
}

void OpenChapterSelect()
{
	g_ChapterSelect.Open();

	gEngfuncs.Key_Event(27, 1);
}

void CChapterSelect::Init()
{
	g_ImGuiManager.AddWindow(this);

	InitChapters();

	gEngfuncs.pfnAddCommand("chapterselect", OpenChapterSelect);

	m_ChapterSelectStrings[ChapterSelectString_StartNewGame] = g_ImGuiManager.GetStringLocale("NLUI_STARTNEWGAME");
	m_ChapterSelectStrings[ChapterSelectString_ChapterSelect] = g_ImGuiManager.GetStringLocale("NLUI_CHAPTERSELECT");
	m_ChapterSelectStrings[ChapterSelectString_Next] = g_ImGuiManager.GetStringLocale("NLUI_NEXT");
	m_ChapterSelectStrings[ChapterSelectString_Back] = g_ImGuiManager.GetStringLocale("NLUI_BACK");
	m_ChapterSelectStrings[ChapterSelectString_Cancel] = g_ImGuiManager.GetStringLocale("NLUI_CANCEL");
	m_ChapterSelectStrings[ChapterSelectString_Chapter] = g_ImGuiManager.GetStringLocale("NLUI_CHAPTER");
	m_ChapterSelectStrings[ChapterSelectString_Easy] = g_ImGuiManager.GetStringLocale("NLUI_EASY");
	m_ChapterSelectStrings[ChapterSelectString_Medium] = g_ImGuiManager.GetStringLocale("NLUI_MEDIUM");
	m_ChapterSelectStrings[ChapterSelectString_Hard] = g_ImGuiManager.GetStringLocale("NLUI_HARD");

	m_bInitialized = true;
}

void CChapterSelect::Shutdown()
{
	m_ChapterInfo.clear();
}

void CChapterSelect::Update()
{
	if (!m_bOpen || !m_bInitialized || NLUtils::ScaleFont() == 0.0f)
		return;

	const char* pszSkillNames[3] = {GetStringByIndex(ChapterSelectString_Easy),
		GetStringByIndex(ChapterSelectString_Medium),
		GetStringByIndex(ChapterSelectString_Hard)};

	int iStartIndex = 0;
	int iEndIndex = 3;

	if (m_iCurrentPage > 0)
	{
		iStartIndex = iEndIndex * m_iCurrentPage;
		iEndIndex = iStartIndex + 3;
	}

	ImVec2 ImVec2LastFramePos;
	ImFont SmallFont = *ImGui::GetFont();
	SmallFont.Scale = 0.8f;

	ImGui::Begin(GetStringByIndex(ChapterSelectString_ChapterSelect), &m_bOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::PushFont(&SmallFont);
	ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
	if (ImGui::BeginTable("Chapters", 3))
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		for (size_t i = iStartIndex; i < iEndIndex; i++)
		{
			if (i >= m_ChapterInfo.size())
			{
				m_ChapterInfo.at(0).texture.Scale();

				ImGui::Text("");
				ImGui::Text("");

				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0, 0, 0, 0));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0, 0, 0, 0));

				ImGui::ImageButton((void*)(intptr_t)9999999, ImVec2(m_ChapterInfo.at(0).texture.width, m_ChapterInfo.at(0).texture.height), ImVec2(0,0), ImVec2(1,1), -1, ImVec4(0,0,0,0), ImVec4(0,0,0,0));
				ImVec2LastFramePos = ImGui::GetCursorPos();
				ImVec2LastFramePos.x += ImGui::GetItemRectSize().x;

				ImGui::PopStyleColor(3);

				ImGui::TableNextColumn();

				continue;
			}

			m_ChapterInfo.at(i).texture.Scale();

			ImGui::TextColored(ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive), "%s %i", GetStringByIndex(ChapterSelectString_Chapter), i + 1);
			ImGui::Text("%s", m_ChapterInfo.at(i).name.c_str());

			bool bPopStyleColors = false;
			
			if (m_pSelectedChapter == &m_ChapterInfo.at(i))
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImGui::GetStyleColorVec4(ImGuiCol_HeaderActive));
				bPopStyleColors = true;
			}
			if (ImGui::ImageButton((void*)(intptr_t)m_ChapterInfo.at(i).texture.texture, ImVec2(m_ChapterInfo.at(i).texture.width, m_ChapterInfo.at(i).texture.height)))
			{
				m_pSelectedChapter = &m_ChapterInfo.at(i);
			}
			if (bPopStyleColors)
			{
				ImGui::PopStyleColor(3);
			}

			ImVec2LastFramePos = ImGui::GetCursorPos();
			ImVec2LastFramePos.x += ImGui::GetItemRectSize().x;

			ImGui::TableNextColumn();
		}
		ImGui::EndTable();
	}
	ImGui::PopStyleVar();
	if (m_iCurrentPage != 0)
	{
		if (ImGui::Button(GetStringByIndex(ChapterSelectString_Back)))
		{
			m_iCurrentPage = V_max(m_iCurrentPage--, 0);
		}
	}

	if (m_iCurrentPage > 0 && m_iCurrentPage < m_iMaxPages)
	{
		ImGui::SameLine();
	}

	if (m_iCurrentPage < m_iMaxPages)
	{
		ImGui::SetCursorPosX(ImVec2LastFramePos.x - NLUtils::CalcButtonSize(GetStringByIndex(ChapterSelectString_Next)).x);
		if (ImGui::Button(GetStringByIndex(ChapterSelectString_Next)))
		{
			m_iCurrentPage = V_min(m_iCurrentPage++, m_iMaxPages);
		}
	}
	ImGui::Dummy(ImVec2(0, ImGui::GetStyle().FramePadding.y));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(0, ImGui::GetStyle().FramePadding.y));

	ImGui::SetNextItemWidth(NLUtils::CalcButtonSize(GetStringByIndex(ChapterSelectString_Medium)).x + ImGui::GetStyle().FramePadding.x*4);
	if (ImGui::Combo("##Skill", &m_iSkill, pszSkillNames, 3))
	{
		gEngfuncs.Cvar_SetValue("skill", m_iSkill + 1);
	}
	ImGui::SameLine();

	ImGui::SetCursorPosX(ImVec2LastFramePos.x - NLUtils::CalcButtonSize(GetStringByIndex(ChapterSelectString_Cancel)).x);
	if (ImGui::Button(GetStringByIndex(ChapterSelectString_Cancel)))
	{
		m_bOpen = false;
	}
	ImGui::SameLine();
	ImGui::SetCursorPosX(ImVec2LastFramePos.x - ImGui::GetStyle().FramePadding.x - NLUtils::CalcButtonSize(GetStringByIndex(ChapterSelectString_StartNewGame)).x 
		- NLUtils::CalcButtonSize(GetStringByIndex(ChapterSelectString_Cancel)).x);
	if (ImGui::Button(GetStringByIndex(ChapterSelectString_StartNewGame)))
	{
		if (m_pSelectedChapter)
		{
			gEngfuncs.pfnClientCmd(((string) "map " + m_pSelectedChapter->maps.at(0) + "\n").c_str());
			m_bOpen = false;
		}
	}

	ImGui::PopFont();

	ImGui::End();
}

void CChapterSelect::InitChapters()
{
	ParseChapters();
	InitThumbnails();
}

void CChapterSelect::ParseChapters()
{
	static char token[512], token2[512];
	char *pfile, *afile;
	pfile = afile = reinterpret_cast<char*>(gEngfuncs.COM_LoadFile("resource/nlui/chapters.txt", 5, 0));

	if (!pfile || !afile)
		return;

	while (pfile = gEngfuncs.COM_ParseFile(pfile, token))
	{
		if (!stricmp(token, "name"))
		{
			auto p = &m_ChapterInfo.emplace_back();

			pfile = gEngfuncs.COM_ParseFile(pfile, token);
			p->nametoken = p->name = token;
			p->name = g_ImGuiManager.GetStringLocale(p->nametoken);

			while (pfile = gEngfuncs.COM_ParseFile(pfile, token))
			{
				if (token[0] == '{')
				{
					while (pfile = gEngfuncs.COM_ParseFile(pfile, token))
					{
						if (token[0] == '}')
							break;
					}
					if (token[0] == '}')
						break;
					continue;
				}
				p->maps.push_back(token);
			}
		}
	}

	gEngfuncs.COM_FreeFile(afile);
}

void CChapterSelect::InitThumbnails()
{
	int i = 0;
	
	const char* path = "resource/nlui/thumbnails/";
	static char filepath[512];

	int remainder = 0;
	
	while (1)
	{
		if (i >= m_ChapterInfo.size())
			break;

		sprintf(filepath, "%s%i.png", path, i);
		if (!NLUtils::LoadTextureFromFile(filepath, &m_ChapterInfo.at(i).texture))
		{
			break;
		}

		m_ChapterInfo.at(i).texture.Scale();

		remainder = ((i + 1) % 3);
		if (remainder == 0)
		{
			m_iMaxPages++;
		}
		i++;
	}
}