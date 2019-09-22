#pragma once

#include "cnpch.h"
#include "Globals.h"
#include "Application.h"


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

	void LogCompilationFirstMessage()
	{
		std::cout <<	"-- Compilation Start Succeeded --"	<<		std::endl			<<
						"	Compilation at Date "			<<		__DATE__			<<
						" and Time "						<<		__TIME__			<<
						std::endl;

		//Checking and printing C++ version used by compiler
		std::string cppVersion = GetCppVersion(__cplusplus);
		std::cout <<	"	Standard C++ Version Implemented by Compiler: "		<< __cplusplus	<< " ("			<< cppVersion << ")" << std::endl <<	   
						"	OS Found = " << (__STDC_HOSTED__ ? "Yes" : "No")	<< std::endl	<< std::endl	<< std::endl; //Print if found (or not) an OS

		//__cplusplus returning values:
		//199711L (C++98 or C++03)
		//201103L (C++11)
		//201402L (C++14)
		//201703L (C++17)

		//OpenGL and GPU Info Print
		std::cout << std::endl << std::endl << "---- GPU HARDWARE & OPEN GL INFO LOG -----------" << std::endl << std::endl;
		std::cout <<		
					 "	OpenGL Version: "	<<	glGetString(GL_VERSION)		<< std::endl								<<
					 "	GPU Benchmark: "	<<	glGetString(GL_VENDOR)		<< std::endl								<<
					 "		  GPU: "		<<	glGetString(GL_RENDERER)	<< std::endl	<<		std::endl			<<
					 "	OpenGL Shading Language Version: "					<< glGetString(GL_SHADING_LANGUAGE_VERSION)	<< std::endl <<

		std::endl << "---- END OF GPU HARDWARE & OPEN GL INFO LOG ----" << std::endl << std::endl << std::endl;


		//CPU Info Print
		cppVersion = GetCppVersion(_MSVC_LANG);
		std::cout << std::endl << std::endl << "---- CPU HARDWARE INFO LOG -----------" << std::endl << std::endl;
		std::cout <<

					"	PC RAM Storage: "		<<		(float)SDL_GetSystemRAM() / (1024.0f)		<<	" GB"			<< std::endl <<		// GB of RAM memory
					"	Avalable CPU Cores: "	<<		SDL_GetCPUCount()							<<	std::endl		<<					// Number of available cores in CPU
					"	L1 Cache Line Size: "	<<		SDL_GetCPUCacheLineSize()					<<	" Bytes"		<< std::endl <<		// Size of the L1 line (little but fast) of the cache memory
					"	RDTSC Available: "		<<		(SDL_HasRDTSC() ? "Yes" : "No")				<<	std::endl		<<					// If RDTSC is (or not) available - (Read Time Stamp Counter - Records CPU cycles since reset)
					"	VS Compiler Version: "	<<		_MSC_VER									<<	std::endl		<<					// Compiler Version
					"	C++ Version Used: "		<<		_MSVC_LANG << " (" << cppVersion << ")"		<<	std::endl		<<					// C++ standard version targeted by compiler
					"	Multithreaded Specified: "		<<				(_MT ? "Yes" : "No")		<<	std::endl		<<					// If multithreaded is specified (or not)

		std::endl << "---- END OF CPU HARDWARE INFO LOG ----" << std::endl << std::endl;



		SYSTEM_INFO SystemInfo;
		GetSystemInfo(&SystemInfo);

		std::cout << "Number of Processors: " << SystemInfo.dwNumberOfProcessors << std::endl;

		
		SystemInfo.wProcessorArchitecture;
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

		std::cout << "Processor Architecture: " << processorArch << std::endl;
		std::cout << "Processor Revision: " << SystemInfo.wProcessorRevision << std::endl;

		//Memory Info
		MEMORYSTATUSEX MemoryInfo;
		MemoryInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&MemoryInfo);

		float div = (1e+9);

		std::cout << std::endl;
		std::cout << "Percentage of Memory in Use: "		<<				 MemoryInfo.dwMemoryLoad				<<	" %"	<< std::endl;
		std::cout << "Total physical memory: "				<<				 MemoryInfo.ullTotalPhys/div			<<	" GB"	<< std::endl;
		std::cout << "Free physical memory: "				<<				 MemoryInfo.ullAvailPhys/div			<<	" GB"	<< std::endl;
		std::cout << "Used physical memory: "				<<	 (MemoryInfo.ullTotalPhys - MemoryInfo.ullAvailPhys)/div	<< " GB" << std::endl;
		std::cout << std::endl;
		std::cout << "Total virtual memory: "				<<				 MemoryInfo.ullTotalVirtual/div			<<	" GB"	<< std::endl;
		std::cout << "Free virtual memory: "				<<				 MemoryInfo.ullAvailVirtual/div			<<	" GB"	<< std::endl;
		std::cout << std::endl;
		std::cout << "Free extended memory: "				<<				 MemoryInfo.ullAvailExtendedVirtual/div	<<	" GB"	<< std::endl;
		std::cout << "Total Page File memory: "				<<				 MemoryInfo.ullTotalPageFile/div		<<	" GB"	<< std::endl;
		std::cout << "Free Page File memory: "				<<				 MemoryInfo.ullAvailPageFile/div		<<	" GB"	<< std::endl;


		PROCESS_MEMORY_COUNTERS pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
		SIZE_T virtualMemUsedByMe = pmc.PagefileUsage;
		SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
		std::cout << std::endl;
		std::cout << "Virtual memory used by process: " << virtualMemUsedByMe/div << std::endl;
		std::cout << "Physical memory used by process: " << physMemUsedByMe/div << std::endl;

	}
}