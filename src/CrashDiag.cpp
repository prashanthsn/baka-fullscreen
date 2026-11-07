// Diagnostic only: records serious exceptions (module+offset and call stack) to
// Data\F4SE\Plugins\BakaFullscreenPipboy_crash.log so crashes can be traced.
#include <windows.h>

#include <cstdint>
#include <cstdio>
#include <cstring>

namespace
{
	void Describe(void* a_addr, char* a_out, std::size_t a_size)
	{
		HMODULE mod = nullptr;
		if (GetModuleHandleExA(
				GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
				static_cast<LPCSTR>(a_addr),
				&mod) &&
			mod) {
			char path[MAX_PATH]{};
			GetModuleFileNameA(mod, path, MAX_PATH);
			const char* name = std::strrchr(path, '\\');
			name = name ? name + 1 : path;
			std::snprintf(a_out, a_size, "%s+0x%llx", name,
				static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(a_addr) - reinterpret_cast<std::uintptr_t>(mod)));
		} else {
			std::snprintf(a_out, a_size, "unknown@0x%llx",
				static_cast<unsigned long long>(reinterpret_cast<std::uintptr_t>(a_addr)));
		}
	}

	LONG WINAPI Handler(EXCEPTION_POINTERS* a_ep)
	{
		const auto code = a_ep->ExceptionRecord->ExceptionCode;
		if (code != 0xC0000005 && code != 0xC0000094 && code != 0xC00000FD &&
			code != 0xC000001D && code != 0xC0000374 && code != 0xC0000409) {
			return EXCEPTION_CONTINUE_SEARCH;
		}

		static volatile LONG count = 0;
		if (InterlockedIncrement(&count) > 40) {
			return EXCEPTION_CONTINUE_SEARCH;
		}

		static char buf[12288];
		int        n = 0;
		char       d[320];

		Describe(a_ep->ExceptionRecord->ExceptionAddress, d, sizeof(d));
		n += std::snprintf(buf + n, sizeof(buf) - n,
			"=== exception 0x%08lX at %s  (tick %llu, thread %lu)\n",
			static_cast<unsigned long>(code), d,
			static_cast<unsigned long long>(GetTickCount64()), GetCurrentThreadId());

		if (code == 0xC0000005 && a_ep->ExceptionRecord->NumberParameters >= 2) {
			n += std::snprintf(buf + n, sizeof(buf) - n, "    access violation: %s address 0x%llx\n",
				a_ep->ExceptionRecord->ExceptionInformation[0] == 0 ? "READ" : (a_ep->ExceptionRecord->ExceptionInformation[0] == 1 ? "WRITE" : "EXEC"),
				static_cast<unsigned long long>(a_ep->ExceptionRecord->ExceptionInformation[1]));
		}

		CONTEXT ctx = *a_ep->ContextRecord;
		for (int i = 0; i < 24 && n < static_cast<int>(sizeof(buf)) - 400; ++i) {
			Describe(reinterpret_cast<void*>(ctx.Rip), d, sizeof(d));
			n += std::snprintf(buf + n, sizeof(buf) - n, "    #%02d %s\n", i, d);

			DWORD64 base = 0;
			auto    fn = RtlLookupFunctionEntry(ctx.Rip, &base, nullptr);
			__try {
				if (!fn) {
					if (!ctx.Rsp) break;
					ctx.Rip = *reinterpret_cast<DWORD64*>(ctx.Rsp);
					ctx.Rsp += 8;
				} else {
					PVOID   handlerData = nullptr;
					DWORD64 establisher = 0;
					RtlVirtualUnwind(UNW_FLAG_NHANDLER, base, ctx.Rip, fn, &ctx, &handlerData, &establisher, nullptr);
				}
			} __except (EXCEPTION_EXECUTE_HANDLER) {
				break;
			}
			if (!ctx.Rip) break;
		}

		HANDLE h = CreateFileA("Data\\F4SE\\Plugins\\BakaFullscreenPipboy_crash.log", FILE_APPEND_DATA,
			FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
		if (h != INVALID_HANDLE_VALUE) {
			DWORD written = 0;
			WriteFile(h, buf, static_cast<DWORD>(n), &written, nullptr);
			FlushFileBuffers(h);
			CloseHandle(h);
		}

		return EXCEPTION_CONTINUE_SEARCH;
	}
}

void InstallCrashDiag()
{
	AddVectoredExceptionHandler(1, Handler);
}
