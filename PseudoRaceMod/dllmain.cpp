// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "IniFile.hpp"
#include <vector>
#include <list>
#include <sstream>
#include <iomanip>
#include <string>

extern "C"
{
	__declspec(dllexport) void __cdecl Init(const char *path, const HelperFunctions &helperFunctions)
	{
		const IniFile *config = new IniFile(std::string(path) + "\\config.ini");
		bool esgOn = config->getBool("Speedups", "ESG", true);
		bool inLevelSkipOn = config->getBool("Speedups", "InLevelCutsceneSkip", true);
		bool cutsceneSkipOn = config->getBool("Speedups", "CutsceneSkip", true);

		if (esgOn) {
			*(int *)0x0174b08c = 1;
		}

		if (inLevelSkipOn) {
			// Change JZ to JMP
			WriteData<1>((void *)0x458978, (uint8_t)0xeb);
		}

		if (cutsceneSkipOn) {
			//Case 0: Set game mode to 0xc
			WriteData<1>((void *)0x458a51, (uint8_t)0x0c);
			//Case 0: clear next_game_mode instruction into NOPs
			WriteData<10>((void *)0x458a55, 0x90);
			//Case 0: Write `inc byte ptr [0x1deb31e]`
			uint8_t incInst[6] = { 0xfe, 0x05, 0x1e, 0xb3, 0xde, 0x01 };
			WriteData((void *)0x458a55, incInst);

			//Case 3: Set game mode to 0xc
			WriteData<1>((void *)0x458b32, 0x0c);
			//Case 3: Clear next_game_mode instruction into NOPs
			WriteData<10>((void *)0x458b36, 0x90);
			//Case 3: write `inc byte ptr [0x1deb31e]`
			WriteData((void *)0x458b36, incInst);
		}
	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer }; // This is needed for the Mod Loader to recognize the DLL.
}
