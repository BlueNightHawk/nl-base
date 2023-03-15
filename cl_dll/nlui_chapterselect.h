#pragma once

typedef struct ChapterInfo_s
{
	string nametoken;
	string name;
	std::vector<string> maps;
	gltexture_s texture;
} ChapterInfo_t;


typedef enum chapterselectstrings_e
{
	ChapterSelectString_StartNewGame,
	ChapterSelectString_ChapterSelect,
	ChapterSelectString_Next,
	ChapterSelectString_Back,
	ChapterSelectString_Cancel,
	ChapterSelectString_Chapter,
	ChapterSelectString_Easy,
	ChapterSelectString_Medium,
	ChapterSelectString_Hard,
	ChapterSelectString_End
} chapterselectstrings_e;

class CChapterSelect : public CImGuiWindow
{
public:
	void Init() override;
	void Shutdown() override;

	void Update() override;

	void Open();

private:
	std::vector<ChapterInfo_s> m_ChapterInfo;

	string m_ChapterSelectStrings[ChapterSelectString_End];

	void InitChapters();
	void ParseChapters();
	void InitThumbnails();

	const char* GetStringByIndex(chapterselectstrings_e idx)
	{
		return m_ChapterSelectStrings[idx].c_str();
	}

	int m_iCurrentPage;
	int m_iMaxPages;

	ChapterInfo_s* m_pSelectedChapter;

	bool m_bInitialized = false;

	int m_iSkill = 0;
};

extern CChapterSelect g_ChapterSelect;