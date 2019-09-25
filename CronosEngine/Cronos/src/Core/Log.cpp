#pragma once

#include "cnpch.h"
#include "Globals.h"
#include "Application.h"
#include "../ImGui/ImGuiLayer.h"
#include "psapi.h"


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


	const std::string WindowsVersion() {

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


	void LogCompilationFirstMessage()
	{
		App->EditorGUI->TestLog <<	"-- Compilation Start Succeeded --"	<<		std::endl			<<
						"	Compilation at Date "			<<		__DATE__			<<
						" and Time "						<<		__TIME__			<<
		std::endl;

		//---------------------------------- CHECKING AND PRINTING C++ VERSION USED BY COMPILER ---------------------------------------------
		std::string cppVersion = GetCppVersion(__cplusplus);
		App->EditorGUI->TestLog <<	"	Standard C++ Version Implemented by Compiler: "		<< __cplusplus							<< " ("			<< cppVersion << ")" << std::endl <<	   
						"	OS Found: "											<< (__STDC_HOSTED__ ? "Yes" : "No")		<< std::endl	<<
						"	OS Version: "										<< WindowsVersion() << std::endl		<< std::endl
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
		glGetIntegerv(0x9048, &GPU_TotalMem);
		glGetIntegerv(0x9049, &GPU_CurrentMem);

		App->EditorGUI->TestLog << std::endl << std::endl << "---- GPU HARDWARE & OPEN GL INFO LOG -----------" << std::endl << std::endl;
		App->EditorGUI->TestLog <<		
					"	GPU Benchmark: "							<< glGetString(GL_VENDOR)						<< std::endl						<<
					"	GPU Model:     "							<< glGetString(GL_RENDERER)						<< std::endl						<<
					"	GPU Total VRAM Memory: "					<< GPU_TotalMem									<< "KB (ONLY FOR NVIDIA GPUs!)"		<< std::endl <<
					"	GPU Current Availale VRAM Memory: "			<< GPU_CurrentMem								<< "KB (ONLY FOR NVIDIA GPUs!)"		<< std::endl << std::endl <<
					"	OpenGL Shading Language Version: "			<< glGetString(GL_SHADING_LANGUAGE_VERSION)		<< std::endl <<						//Version of GLSL supported
					"	OpenGL Version: "							<< glGetString(GL_VERSION)						<< std::endl <<
																														 
		std::endl << "---- END OF GPU HARDWARE & OPEN GL INFO LOG ----" << std::endl << std::endl << std::endl;


		//------------------------------------------------------------------------------------------------------------------------------------
		//---------------------------------------------------- CPU INFO PRINT ----------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------
		
	
		cppVersion = GetCppVersion(_MSVC_LANG);
		App->EditorGUI->TestLog << std::endl << std::endl << "---- CPU HARDWARE INFO LOG -----------" << std::endl << std::endl;
		App->EditorGUI->TestLog <<

					"	PC RAM Storage: "		<<		(float)SDL_GetSystemRAM() / (1024.0f)		<<	" GB"			<< std::endl <<		// GB of RAM memory
					"	Avalable CPU Cores: "	<<		SDL_GetCPUCount()							<<	std::endl		<<					// Number of available cores in CPU
					"	L1 Cache Line Size: "	<<		SDL_GetCPUCacheLineSize()					<<	" Bytes"		<< std::endl <<		// Size of the L1 line (little but fast) of the cache memory
					"	RDTSC Available: "		<<		(SDL_HasRDTSC() ? "Yes" : "No")				<<	std::endl		<<					// If RDTSC is (or not) available - (Read Time Stamp Counter - Records CPU cycles since reset)
					"	VS Compiler Version: "	<<		_MSC_VER									<<	std::endl		<<					// Compiler Version
					"	C++ Version Used: "		<<		_MSVC_LANG << " (" << cppVersion << ")"		<<	std::endl		<<					// C++ standard version targeted by compiler
					"	Multithreaded Specified: "		<<				(_MT ? "Yes" : "No")		<<	std::endl		<< std::endl;		// If multithreaded is specified (or not)

		SYSTEM_INFO SystemInfo;
		GetSystemInfo(&SystemInfo);

		App->EditorGUI->TestLog << "	Number of Processors: " << SystemInfo.dwNumberOfProcessors << std::endl;
		std::string processorArch = "Unknown architecture";

		switch (SystemInfo.wProcessorArchitecture) {

			case(PROCESSOR_ARCHITECTURE_AMD64):
				processorArch = "x64 (AMD or Intel)";
				break;
			case(PROCESSOR_ARCHITECTURE_ARM):
				processorArch = "ARM";
				break;
			case(PROCESSOR_ARCHITECTURE_ARM64):
				processorArch = "ARM64";
				break;
			case(PROCESSOR_ARCHITECTURE_IA64):
				processorArch = "Intel Itanium-based";
				break;
			case(PROCESSOR_ARCHITECTURE_INTEL):
				processorArch = "x86";
				break;
			case(PROCESSOR_ARCHITECTURE_UNKNOWN):
				processorArch = "Unknown architecture";
				break;
			default:
				processorArch = "Unknown architecture";
				break;
		}

		App->EditorGUI->TestLog << "	Processor Architecture: "	<< processorArch					<< std::endl;
		App->EditorGUI->TestLog << "	Processor Revision: "		<< SystemInfo.wProcessorRevision	<< std::endl;

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

		App->EditorGUI->TestLog << "	CPU: " << CPUBrandString << std::endl;
		App->EditorGUI->TestLog << std::endl << "---- END OF CPU HARDWARE INFO LOG ----" << std::endl << std::endl;

		
		//------------------------------------------------------------------------------------------------------------------------------------
		//---------------------------------------------------- MEMORU INFO PRINT -------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------
		MEMORYSTATUSEX MemoryInfo;
		MemoryInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&MemoryInfo);

		float div = (1e+9);

		App->EditorGUI->TestLog << std::endl << std::endl << "---- MEMORY HARDWARE INFO LOG -----------"							<< std::endl << std::endl;
		
		App->EditorGUI->TestLog << "	Percentage of Memory in Use: "		<<				 MemoryInfo.dwMemoryLoad				<<	" %"	<< std::endl;
		App->EditorGUI->TestLog << "	Total physical memory: "			<<				 MemoryInfo.ullTotalPhys/div			<<	" GB"	<< std::endl;
		App->EditorGUI->TestLog << "	Free physical memory: "				<<				 MemoryInfo.ullAvailPhys/div			<<	" GB"	<< std::endl;
		App->EditorGUI->TestLog << "	Used physical memory: "				<<	 (MemoryInfo.ullTotalPhys - MemoryInfo.ullAvailPhys)/div		<< " GB" << std::endl;
		App->EditorGUI->TestLog << std::endl;
		App->EditorGUI->TestLog << "	Total virtual memory: "				<<				 MemoryInfo.ullTotalVirtual/div			<<	" GB"	<< std::endl;
		App->EditorGUI->TestLog << "	Free virtual memory: "				<<				 MemoryInfo.ullAvailVirtual/div			<<	" GB"	<< std::endl;
		App->EditorGUI->TestLog << std::endl;
		App->EditorGUI->TestLog << "	Free extended memory: "				<<				 MemoryInfo.ullAvailExtendedVirtual/div	<<	" GB"	<< std::endl;
		App->EditorGUI->TestLog << "	Total Page File memory: "			<<				 MemoryInfo.ullTotalPageFile/div		<<	" GB"	<< std::endl;
		App->EditorGUI->TestLog << "	Free Page File memory: "			<<				 MemoryInfo.ullAvailPageFile/div		<<	" GB"	<< std::endl;


		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		SIZE_T virtualMemUsedByMe = pmc.PagefileUsage;
		SIZE_T physMemUsedByMe = pmc.WorkingSetSize;

		App->EditorGUI->TestLog << std::endl;
		App->EditorGUI->TestLog << "	Virtual memory used by process: "	<< virtualMemUsedByMe/div	<< " GB" << std::endl;
		App->EditorGUI->TestLog << "	Physical memory used by process: "	<< physMemUsedByMe/div		<< " GB" << std::endl;
		
		//------------------------------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------------------------------
		App->EditorGUI->TestLog << std::endl << "---- END OF MEMORY HARDWARE INFO LOG ----" << std::endl;
		App->EditorGUI->TestLog << std::endl << std::endl;
	}
}