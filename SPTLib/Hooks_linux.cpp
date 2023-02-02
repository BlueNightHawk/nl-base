#include "sptlib-stdafx.hpp"

#include "sptlib.hpp"
#include "MemUtils.hpp"
#include "Hooks.hpp"
#include <dlfcn.h>
#include <link.h>


namespace Hooks
{
	static std::mutex& dl_refcount_mutex() {
		static std::mutex value;
		return value;
	};

	static std::unordered_map<void*, size_t>& dl_refcount() {
		static std::unordered_map<void*, size_t> value;
		return value;
	}

	static void* get_dlsym_addr(const wchar_t *library)
	{
		return nullptr;
	}
#if 0
	extern "C" void* dlopen(const char* filename, int flag)
	{
		if (!ORIG_dlopen)
			ORIG_dlopen = reinterpret_cast<_dlopen>(dlsym(RTLD_NEXT, "dlopen"));

		auto rv = ORIG_dlopen(filename, flag);
		if (DebugEnabled())
			EngineDevMsg("Engine call: dlopen( \"%s\", %d ) => %p\n", filename, flag, rv);

		if (rv) {
			std::lock_guard<std::mutex> lock(dl_refcount_mutex());
			++dl_refcount()[rv];
		}

		if (rv && filename)
			HookModule(Convert(filename));

		return rv;
	}

	extern "C" int dlclose(void* handle)
	{
		if (!ORIG_dlclose)
			ORIG_dlclose = reinterpret_cast<_dlclose>(dlsym(RTLD_NEXT, "dlclose"));

		bool unhook = false;
		{
			std::lock_guard<std::mutex> lock(dl_refcount_mutex());

			auto refcount_it = dl_refcount().find(handle);
			if (refcount_it != dl_refcount().end()) {
				if (refcount_it->second > 0)
					--refcount_it->second;

				if (refcount_it->second == 0)
					unhook = true;
			}
		}

		if (unhook)
			for (auto it = modules.cbegin(); it != modules.cend(); ++it)
				if ((*it)->GetHandle() == handle)
					(*it)->Unhook();

		auto rv = ORIG_dlclose(handle);
		if (DebugEnabled())
			EngineDevMsg("Engine call: dlclose( %p ) => %d\n", handle, rv);

		return rv;
	}

	extern "C" void* dlsym(void* handle, const char* name)
	{
		if (!ORIG_dlsym)
			// GLIBC >= 2.34
			ORIG_dlsym = reinterpret_cast<_dlsym>(get_dlsym_addr(L"libc.so"));
		if (!ORIG_dlsym)
			// GLIBC <= 2.33
			ORIG_dlsym = reinterpret_cast<_dlsym>(get_dlsym_addr(L"libdl.so"));
		assert(ORIG_dlsym);

		auto rv = ORIG_dlsym(handle, name);

		auto result = MemUtils::GetSymbolLookupResult(handle, rv);
		if (DebugEnabled())
		{
			if (result != rv)
				EngineDevMsg("Engine call: dlsym( %p, %s ) => %p [returning %p]\n", handle, name, rv, result);
			else
				EngineDevMsg("Engine call: dlsym( %p, %s ) => %p\n", handle, name, rv, result);
		}

		return result;
	}
	#endif
	void InitInterception(bool needToIntercept)
	{
	}

	void ClearInterception(bool needToIntercept)
	{
	}
}
