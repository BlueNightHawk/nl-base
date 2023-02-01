#include "sptlib-stdafx.hpp"

#include "DetoursUtils.hpp"
#include <cassert>

namespace DetoursUtils
{
using namespace std;

std::vector<subhook::Hook> m_Hooks;

void AttachDetours(const wstring& moduleName, size_t n, const pair<void**, void*> funcPairs[])
{
	size_t hook_count = 0;
	for (size_t i = 0; i < n; ++i)
	{
		void** target = funcPairs[i].first;
		void* detour = funcPairs[i].second;
		assert(target);

		if (*target && detour)
		{
			void* original = *target;
		//	auto status = MH_CreateHook(original, detour, target);
			m_Hooks.emplace_back();
			m_Hooks.back().Install(original, detour);
			if (!m_Hooks.back().IsInstalled())
			{
				continue;
			}
			hook_count++;
		}
	}

	if (hook_count == 0)
	{
		return;
	}
}

void DetachDetours(const wstring& moduleName, size_t n, void** const functions[])
{
	size_t hook_count = 0;
	for (size_t i = 0; i < n; ++i)
	{
		void** tramp = functions[i];
		assert(tramp);

		if (*tramp)
		{
			void* original;
			{
				lock_guard<mutex> lock(tramp_to_original_mutex);
				original = tramp_to_original[*tramp];
				tramp_to_original.erase(*tramp);
			}

			auto status = MH_RemoveHook(original);
			if (status != MH_OK)
			{
				continue;
			}

			*tramp = original;
			hook_count++;
		}
	}

	if (!hook_count)
	{
		return;
	}
}
}
