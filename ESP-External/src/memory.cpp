#include "memory.h"
#include <TlHelp32.h>

Memory::Memory(const char* name)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry {};
		procEntry.dwSize = sizeof(PROCESSENTRY32);

		while (Process32Next(hSnap, &procEntry))
		{
			if (!strcmp(name, procEntry.szExeFile))
			{
				this->id = procEntry.th32ProcessID;
				this->hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, this->id);
				break;
			}
		}
	}

	CloseHandle(hSnap);
}

uintptr_t Memory::getModuleAddress(const char* name)
{
	HANDLE mSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, this->id);

	uintptr_t module {};

	if (mSnap != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 modEntry {};
		modEntry.dwSize = sizeof(MODULEENTRY32);

		while (Module32Next(mSnap, &modEntry))
		{
			if (!strcmp(name, modEntry.szModule))
			{
				module = reinterpret_cast<uintptr_t>(modEntry.modBaseAddr);
				break;
			}
		}
	}

	CloseHandle(mSnap);
	return module;
}
