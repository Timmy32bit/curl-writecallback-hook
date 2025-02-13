#include <Windows.h>

#include "minhook/MinHook.h"
#include <cstdint>

#define TRAMP(returnType, callingConvention, functionName, ...) \
    typedef returnType(callingConvention* functionName##_fpt)(__VA_ARGS__); \
    functionName##_fpt functionName##_fp = NULL;

#define _QWORD uint64_t
#define _DWORD DWORD
#define _WORD WORD
#define __int128 unsigned long long

TRAMP(__int64, __fastcall, sub_180044770, __int64, __int64, void**)
TRAMP(__int64, __fastcall, writecallback, void*, size_t, size_t, void*)

__int64 __fastcall writecallback_hook(void* contents, size_t size, size_t nmemb, void* userp) {

    constexpr char replacement[] = R"("fuck bitches, get money")";

    char* buffer = static_cast<char*>(contents);
    memcpy(buffer, replacement, sizeof(replacement) - 1);

    return writecallback_fp(contents, size, nmemb, userp);
}

__int64 __fastcall sub_180044770_hook(__int64 a1, __int64 a2, void** a3) {

    auto fwrite = (uint64_t)GetProcAddress(GetModuleHandleA("ucrtbase.dll"), "fwrite");
    auto result = sub_180044770_fp(a1, a2, a3);

    if (*(_QWORD*)(a1 + 592) != fwrite) {
        if (!writecallback_fp) {
            MH_CreateHook((LPVOID)(*(_QWORD*)(a1 + 592)), writecallback_hook, reinterpret_cast<LPVOID*>(&writecallback_fp));
            MH_EnableHook(MH_ALL_HOOKS);
        }
    }

    return result;
}

int main() {

    MH_Initialize();
    MH_CreateHook((LPVOID)((uintptr_t)GetModuleHandleW(L"libcurl.dll") + 0x44770), sub_180044770_hook, reinterpret_cast<LPVOID*>(&sub_180044770_fp));
    MH_EnableHook(MH_ALL_HOOKS);

    return 1;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        main();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

