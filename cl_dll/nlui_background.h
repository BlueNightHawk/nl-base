#pragma once

class CBackground
{
public:
	void Init();
	void Update();
	void Shutdown();

private:
	gltexture_s m_Background;
};