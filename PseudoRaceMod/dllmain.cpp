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
		bool resultsScreenSkipOn = config->getBool("Speedups", "ResultsSkip", true);
		bool stageNameSkipOn = config->getBool("Speedups", "StageNameSkip", true);

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

		if (resultsScreenSkipOn) {
			// CalcTotalScore
			// Don't display HUD function
			WriteData<7>((void *)0x44f845, 0x90);
			// Go to state 0x2 right after state 0x0
			// to do the rank calc
			WriteData<1>((void *)0x44f84e, 0x2);
			// Skip the fade in state 2
			WriteData<1>((void*)0x44f8c4, 0xff);
			// Go to state 0xc
			// for part 2 of rank calc
			WriteData<1>((void*)0x44f8eb, 0xc);
			// Skip the score countup
			WriteData<2>((void*)0x44fae5, 0x90);
			WriteData<2>((void*)0x44faeb, 0x90);
			// Set next state to 0x10
			WriteData<1>((void*)0x44fb0a, 0x10);
			// Skip cash register sfx (2 spots)
			WriteData<5>((void*)0x44fb0e, 0x90);
			WriteData<5>((void*)0x44fba0, 0x90);
			// Skip loop in state 0xc
			WriteData<0x2f>((void*)0x44fb1e, 0x90);
			// Skip saving file in state 0xc
			WriteData<5>((void*)0x44fb63, 0x90);
			// Skip random function at the end of 0x10
			WriteData<5>((void*)0x44fcec, 0x90);

			// CalcTotalTime
			// Don't display HUD function
			WriteData<7>((void *)0x450805, 0x90);
			// Go to state 0x2 right after state 0x0
			// To do the rank calc
			WriteData<1>((void *)0x450812, 0x2);
			// Skip fade in state 2
			WriteData<1>((void*)0x450876, 0xff);
			// Go to state 0x7
			// for part 2 of rank calc
			WriteData<1>((void*)0x4508ac, 0x7);
			// Skip score window/display total rings
			WriteData<5>((void*)0x4508ca, 0x90);
			WriteData<5>((void*)0x4508df, 0x90);
			// Skip delay in state 7
			WriteData<1>((void*)0x4509c3, 0x00);
			// Skip save in state 7
			WriteData<5>((void*)0x4509e4, 0x90);

			// NotCalcTotalScore
			// Go to state 0x10 right after state 0x0
			WriteData<1>((void *)0x45100e, 0x10);



		}

		if (stageNameSkipOn) {
			// StageNameDisplayExecutor
			// case 0 disp_delayed_4 func clear
			WriteData<7>((void *)0x472530, 0x90);
			// case 0 into case 2
			WriteData<1>((void *)0x47252f, 0x2);
			// case 2 goes into case 6
			WriteData<1>((void *)0x472641, 0x6);
			// case 2 go to case 3 right away
			WriteData<1>((void *)0x472635, 0x0);
			// case 6 exit quick
			WriteData<1>((void *)0x47287d, 0x0);

			// Don't create mission window
			WriteData<5>((void *)0x472673, 0x90);

			// Don't create spinball
			WriteData<5>((void *)0x472b23, 0x90);
			// Don't create static ball
			WriteData<5>((void *)0x472b33, 0x90);
			// Don't create left shape
			WriteData<5>((void *)0x472b43, 0x90);
			// Don't create zoom text
			WriteData<5>((void *)0x472b53, 0x90);
			// Don't create up scroll text
			WriteData<5>((void *)0x472b63, 0x90);
			// Don't create stage number
			WriteData<5>((void *)0x472b73, 0x90);
		}
 	}

	__declspec(dllexport) ModInfo SA2ModInfo = { ModLoaderVer }; // This is needed for the Mod Loader to recognize the DLL.
}
