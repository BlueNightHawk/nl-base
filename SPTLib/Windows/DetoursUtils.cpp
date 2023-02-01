#include "../sptlib-stdafx.hpp"

#include "../sptlib.hpp"
#include "DetoursUtils.hpp"
#include "../subhook/subhook.h"

namespace DetoursUtils
{
	using namespace std;

	static map<void*, void*> tramp_to_original;
	static mutex tramp_to_original_mutex;

	void AttachDetours(const wstring& moduleName, size_t n, const pair<void**, void*> funcPairs[])
	{
	}

	void DetachDetours(const wstring& moduleName, size_t n, void** const functions[])
	{
	}
}
