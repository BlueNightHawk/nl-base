#ifdef REGS_ENGINEHOOK_H_RECURSE_GUARD
#error Recursive header files inclusion detected in reGS_enginehook.h
#else //REGS_ENGINEHOOK_H_RECURSE_GUARD

#define REGS_ENGINEHOOK_H_RECURSE_GUARD

#ifndef REGS_ENGINEHOOK_H_GUARD
#define REGS_ENGINEHOOK_H_GUARD
#pragma once

bool HWHook();

extern Utils utils;

#ifdef WIN32
#define Find(future_name)                                                                                                                     \
	{                                                                                                                                         \
		auto f##future_name = utils.FindAsync(ORIG_##future_name, patterns::engine::future_name);                                             \
		auto pattern = f##future_name.get();                                                                                                  \
		if (ORIG_##future_name)                                                                                                               \
		{                                                                                                                                     \
			gEngfuncs.Con_DPrintf("[%s] Found " #future_name " at %p (using the %s pattern).\n", HWEXT, ORIG_##future_name, pattern->name()); \
		}                                                                                                                                     \
	}

#define Hook(future_name, hook)                                                                                                                  \
	{                                                                                                                                      \
		auto f##future_name = utils.FindAsync(ORIG_##future_name, patterns::engine::future_name);                                          \
		auto pattern = f##future_name.get();                                                                                               \
		if (ORIG_##future_name)                                                                                                            \
		{                                                                                                                                  \
			gEngfuncs.Con_DPrintf("[%s] Found " #future_name " at %p (using the %s pattern).\n", HWEXT, ORIG_##future_name, pattern->name()); \
			void* p##future_name = (void*)ORIG_##future_name;                                                                              \
			hook.Install(p##future_name, (void*)future_name);                                              \
		}                                                                                                                                  \
		else                                                                                                                               \
		{                                                                                                                                  \
			gEngfuncs.Con_DPrintf("[%s] Could not find " #future_name ".\n", HWEXT);                                                          \
		}                                                                                                                                  \
	}
#else
#define Find(future_name)                                                                                                                     \
	{                                                                                                                                         \
		ORIG_##future_name = decltype(ORIG_##future_name)(PL_GetProcAddress(utils.GetHandle(), #future_name));                                             \
		if (ORIG_##future_name)                                                                                                               \
		{                                                                                                                                     \
			gEngfuncs.Con_DPrintf("[%s] Found " #future_name " at %p (using the %s pattern).\n", HWEXT, ORIG_##future_name); \
		}                                                                                                                                     \
	}

#define Hook(future_name, hook)                                                                                                               \
	{                                                                                                                                         \
		ORIG_##future_name = decltype(ORIG_##future_name)(PL_GetProcAddress(utils.GetHandle(), #future_name));                                  \
		if (ORIG_##future_name)                                                                                                             \
		{                                                                                                                                     \
			gEngfuncs.Con_DPrintf("[%s] Found " #future_name " at %p.\n", HWEXT, ORIG_##future_name); \
			void* p##future_name = (void*)ORIG_##future_name;                                                                                 \
			hook.Install(p##future_name, (void*)future_name);                                                                                 \
		}                                                                                                                                     \
		else                                                                                                                                  \
		{                                                                                                                                     \
			gEngfuncs.Con_DPrintf("[%s] Could not find " #future_name ".\n", HWEXT);                                                          \
		}                                                                                                                                     \
	}
#endif

#endif //REGS_ENGINEHOOK_H_GUARD

#undef REGS_ENGINEHOOK_H_RECURSE_GUARD
#endif //REGS_ENGINEHOOK_H_RECURSE_GUARD