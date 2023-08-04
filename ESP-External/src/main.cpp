#include "memory.h"
#include <thread>
#include <iostream>

namespace offsets
{
	// client
	constexpr ::std::ptrdiff_t dwLocalPlayer = 0xDEA98C;
	constexpr ::std::ptrdiff_t dwEntityList = 0x4DFFF7C;
	constexpr ::std::ptrdiff_t dwGlowObjectManager = 0x535AA08;
	
	// entity
	constexpr ::std::ptrdiff_t m_iTeamNum = 0xF4;
	constexpr ::std::ptrdiff_t m_iGlowIndex = 0x10488;
}

struct Color
{
	constexpr Color(float r, float g, float b, float a = 1.f) noexcept
		: r(r), g(g), b(b), a(a) { }

	float r, g, b, a;
};

int main()
{
	Memory mem { "csgo.exe" };
	std::cout << "[+] Process ID -> " << mem.getProcessId() << '\n';

	const auto client = mem.getModuleAddress("client.dll");
	std::cout << "[+] Client Dll -> 0x" << std::hex << client << std::dec << '\n';

	Color color { 1.f, 0.f, 1.f };

	// hack loop
	while (true)
	{
		const auto localPlayer = mem.Read<std::uintptr_t>(client + offsets::dwLocalPlayer);

		if (!localPlayer) continue;

		// list of objects that can glow
		const auto glowObjManager = mem.Read<std::uintptr_t>(client + offsets::dwGlowObjectManager);

		if (!glowObjManager) continue;

		// matches have 64 people max
		for (int i = 0; i < 64; ++i)
		{
			// size of each entity objs is 0x10
			const auto entity = mem.Read<std::uintptr_t>(client + offsets::dwEntityList + i * 0x10);

			// checking if entity obj's team number is the same as player's team number
			if (mem.Read<std::uintptr_t>(entity + offsets::m_iTeamNum) == mem.Read<std::uintptr_t>(localPlayer + offsets::m_iTeamNum))
				continue;

			const auto glowIndex = mem.Read<std::int32_t>(entity + offsets::m_iGlowIndex);

			// glow objects have a size of 0x38
			mem.Write<Color>(glowObjManager + (glowIndex * 0x38) + 0x8, color);

			mem.Write<bool>(glowObjManager + (glowIndex * 0x38) + 0x27, true);
			mem.Write<bool>(glowObjManager + (glowIndex * 0x38) + 0x28, true);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	return 0;
}