#pragma once

#include "cnpch.h"
#include "Globals.h"
#include "Application.h"
#include "../ImGui/ImGuiLayer.h"
#include "psapi.h"

#define BTOGB (1073741824.0f)
#define KBTOMB 1024.0f//To GB: (1048576.0f)
#define BTOMB (1048576.0f)

namespace Cronos {

	void Log(const char file[], int line, const char* format, ...)
	{
		static char tmp_string[4096];
		static char tmp_string2[4096];
		static va_list  ap;

		// Construct the string from variable arguments
		va_start(ap, format);
		vsprintf_s(tmp_string, 4096, format, ap);
		va_end(ap);
		sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);

		OutputDebugStringA(tmp_string2);

	}


	const std::string GetCppVersion(long int value) {

		std::string cppVersion = "NULL: return value does not match with any C++ version!";
		switch (value) {

		case(199711L):
			cppVersion = "C++ 98 or C++03";
			break;
		case(201103L):
			cppVersion = "C++11";
			break;
		case(201402L):
			cppVersion = "C++14";
			break;
		case(201703L):
			cppVersion = "C++17";
			break;
		default:
			cppVersion = "NULL: return value does not match with any C++ version!";
			break;
		}

		return cppVersion;
	}


	const std::string GetWindowsVersion() {

		OSVERSIONINFOEX OS;
		ZeroMemory(&OS, sizeof(OSVERSIONINFOEX));
		OS.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		GetVersionEx(&(OSVERSIONINFO&)OS);

		std::string ret = "Windows ";

		if (OS.dwMajorVersion == 10)
			ret += "10";
		else if (OS.dwMajorVersion == 6) {

			if (OS.dwMinorVersion == 3)
				ret += "8.1";
			else if (OS.dwMinorVersion == 2)
				ret += "8";
			else if (OS.dwMinorVersion == 1)
				ret += "7";
			else
				ret += "Vista";
		}
		else if (OS.dwMajorVersion == 5) {

			if (OS.dwMinorVersion == 2)
				ret += "XP SP2";
			else if (OS.dwMinorVersion == 1)
				ret += "XP";
			else if (OS.dwMinorVersion == 0)
				ret += "2000";
		}
		else if (OS.dwMajorVersion == 4 || OS.dwMajorVersion == 3)
			ret += "WinNT";
		else
			ret = "WINDOWS VERSION NOT FOUND";

		return ret;
	}

	const std::string GetCPUArchitecture(SYSTEM_INFO& SystemInfo) {

		std::string ret = "Unknown Architecture";

		switch (SystemInfo.wProcessorArchitecture) {

		case(PROCESSOR_ARCHITECTURE_AMD64):
			ret = "x64 (AMD or Intel)";
			break;
		case(PROCESSOR_ARCHITECTURE_ARM):
			ret = "ARM";
			break;
		case(PROCESSOR_ARCHITECTURE_ARM64):
			ret = "ARM64";
			break;
		case(PROCESSOR_ARCHITECTURE_IA64):
			ret = "Intel Itanium-based";
			break;
		case(PROCESSOR_ARCHITECTURE_INTEL):
			ret = "x86";
			break;
		case(PROCESSOR_ARCHITECTURE_UNKNOWN):
			ret = "Unknown architecture";
			break;
		default:
			ret = "Unknown architecture";
			break;
		}

		return ret;
	}

	void PrintCPUBrand() {

		// Get extended ids.
		int CPUInfo[4] = { -1 };
		__cpuid(CPUInfo, 0x80000000);
		unsigned int nExIds = CPUInfo[0];

		// Get the information associated with each extended ID.
		char CPUBrandString[0x40] = { 0 };
		for (unsigned int i = 0x80000000; i <= nExIds; ++i)
		{
			__cpuid(CPUInfo, i);

			// Interpret CPU brand string and cache information.
			if (i == 0x80000002)
				memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));

			else if (i == 0x80000003)
				memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));

			else if (i == 0x80000004)
				memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}

		App->EditorGUI->TestLog << " CPU: " << CPUBrandString << std::endl << std::endl;
	}


	void LogCompilationFirstMessage()
	{
		App->EditorGUI->TestLog <<	"-- Compilation Start Succeeded --"	<<		std::endl			<<
									"	Compilation at Date "			<<		__DATE__			<<
									" and Time "						<<		__TIME__			<<
		std::endl;

		//---------------------------------- CHECKING AND PRINTING C++ VERSION USED BY COMPILER & WINDOWS VERSION ----------------------------
		std::string cppVersion = GetCppVersion(__cplusplus);
		App->EditorGUI->TestLog <<	"	Standard C++ Version Implemented by Compiler: "		<< __cplusplus							<< " ("			<< cppVersion << ")" << std::endl <<
									"	OS Found: "											<< (__STDC_HOSTED__ ? "Yes" : "No")		<< std::endl	<<
									"	OS Version: "										<< GetWindowsVersion() << std::endl		<< std::endl
		<< std::endl;

		///__cplusplus returning values:
		///199711L (C++98 or C++03)
		///201103L (C++11)
		///201402L (C++14)
		///201703L (C++17)

		//------------------------------------------------------------------------------------------------------------------------------------
		//---------------------------------------------- OPENGL AND GPU INFO PRINT -----------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------
		GLint GPU_TotalMem = 0;
		GLint GPU_CurrentMem = 0;
		glGetIntegerv(0x9048, &GPU_TotalMem); //In KB!
		glGetIntegerv(0x9049, &GPU_CurrentMem); //In KB!

		App->EditorGUI->TestLog << std::endl << std::endl << "---- GPU HARDWARE & OPEN GL INFO LOG -----------" << std::endl << std::endl;

		App->EditorGUI->TestLog <<

					"	GPU Benchmark: "								<< glGetString(GL_VENDOR)						<< std::endl						<<
					"	GPU Model:     "								<< glGetString(GL_RENDERER)						<< std::endl						<<
					"	GPU Total VRAM Memory: "						<< GPU_TotalMem/KBTOMB							<< " MB (ONLY FOR NVIDIA GPUs!)"	<< std::endl <<
					"	GPU Current Availale VRAM Memory: "				<< ceil(GPU_CurrentMem/KBTOMB)					<< " MB (ONLY FOR NVIDIA GPUs!)"	<< std::endl << std::endl <<
					"	OpenGL Shading Language Version: "				<< glGetString(GL_SHADING_LANGUAGE_VERSION)		<< std::endl <<						//Version of GLSL supported
					"	OpenGL Version: "								<< glGetString(GL_VERSION)						<< std::endl <<

		std::endl << "---- END OF GPU HARDWARE & OPEN GL INFO LOG ----" << std::endl << std::endl << std::endl;


		//------------------------------------------------------------------------------------------------------------------------------------
		//---------------------------------------------------- CPU INFO PRINT ----------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------
		cppVersion = GetCppVersion(_MSVC_LANG);
		App->EditorGUI->TestLog << std::endl << std::endl << "---- CPU HARDWARE INFO LOG -----------" << std::endl << std::endl;
		
		PrintCPUBrand();

		App->EditorGUI->TestLog <<

					"	PC RAM Storage: "		<<		(float)SDL_GetSystemRAM()/KBTOMB			<<	" GB"			<< std::endl <<		// GB of RAM memory
					"	Avalable CPU Cores: "	<<		SDL_GetCPUCount()							<<	std::endl		<<					// Number of available cores in CPU
					"	L1 Cache Line Size: "	<<		SDL_GetCPUCacheLineSize()					<<	" Bytes"		<< std::endl <<		// Size of the L1 line (little but fast) of the cache memory
					"	RDTSC Available: "		<<		(SDL_HasRDTSC() ? "Yes" : "No")				<<	std::endl		<<					// If RDTSC is (or not) available - (Read Time Stamp Counter - Records CPU cycles since reset)
					"	VS Compiler Version: "	<<		_MSC_VER									<<	std::endl		<<					// Compiler Version
					"	C++ Version Used: "		<<		_MSVC_LANG << " (" << cppVersion << ")"		<<	std::endl		<<					// C++ standard version targeted by compiler
					"	Multithreaded Specified: "		<<			(_MT ? "Yes" : "No")			<<	std::endl		<< std::endl;		// If multithreaded is specified (or not)


		SYSTEM_INFO SystemInfo;
		GetSystemInfo(&SystemInfo);
		std::string processorArch = GetCPUArchitecture(SystemInfo);
		
		App->EditorGUI->TestLog <<

					"	Number of Processors: "		<< SystemInfo.dwNumberOfProcessors	<< std::endl <<
					"	Processor Architecture: "	<< processorArch					<< std::endl <<
					"	Processor Revision: "		<< SystemInfo.wProcessorRevision	<<
		std::endl;
		App->EditorGUI->TestLog <<	std::endl	<< "---- END OF CPU HARDWARE INFO LOG ----" << std::endl << std::endl;


		//------------------------------------------------------------------------------------------------------------------------------------
		//---------------------------------------------------- MEMORU INFO PRINT -------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------
		MEMORYSTATUSEX MemoryInfo;
		MemoryInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&MemoryInfo);

		App->EditorGUI->TestLog << std::endl << std::endl	<< "---- MEMORY HARDWARE INFO LOG -----------"				<< std::endl << std::endl	<<

					"	Percentage of Memory in Use: "		<<				 MemoryInfo.dwMemoryLoad					<<	" %"	<< std::endl	<<
					"	Total physical memory: "			<<				 MemoryInfo.ullTotalPhys/BTOGB				<<	" GB"	<< std::endl	<<
					"	Free physical memory: "				<<				 MemoryInfo.ullAvailPhys/BTOGB				<<	" GB"	<< std::endl	<<
					"	Used physical memory: "				<<	 (MemoryInfo.ullTotalPhys - MemoryInfo.ullAvailPhys)/BTOGB			<< " GB"		<< std::endl <<
					"	Total virtual memory: "				<<				 MemoryInfo.ullTotalVirtual/BTOGB			<<	" GB"	<< std::endl	<<
					"	Free virtual memory: "				<<				 MemoryInfo.ullAvailVirtual/BTOGB			<<	" GB"	<< std::endl	<<
					"	Free extended memory: "				<<				 MemoryInfo.ullAvailExtendedVirtual/BTOMB	<<	" MB"	<< std::endl	<<
					"	Total Page File memory: "			<<				 MemoryInfo.ullTotalPageFile/BTOGB			<<	" GB"	<< std::endl	<<
					"	Free Page File memory: "			<<				 MemoryInfo.ullAvailPageFile/BTOGB			<<	" GB"	<<
		std::endl;


		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		SIZE_T virtualMemUsed = pmc.PagefileUsage;
		SIZE_T physMemUsed = pmc.WorkingSetSize;

		App->EditorGUI->TestLog << std::endl <<
								"	Virtual memory used by process: "	<< virtualMemUsed/BTOMB << " MB" << std::endl <<
								"	Physical memory used by process: "	<< physMemUsed/BTOMB	<< " MB" <<
		std::endl;

		//------------------------------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------
		App->EditorGUI->TestLog << std::endl << "---- END OF MEMORY HARDWARE INFO LOG ----" << std::endl;
		App->EditorGUI->TestLog << std::endl << std::endl;
	}
}
