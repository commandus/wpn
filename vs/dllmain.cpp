// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>

BOOL initWindows()
{
	// Initialize Winsock
	WSADATA wsaData;
	int r = WSAStartup(MAKEWORD(2, 2), &wsaData);
	return r == 0;
}

BOOL APIENTRY DllMain(
	HMODULE hModule,
	DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
	BOOL r;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		r = initWindows();
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
	default:
		r = TRUE;
        break;
    }
    return r;
}
