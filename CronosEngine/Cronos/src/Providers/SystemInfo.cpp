#include "Providers/cnpch.h"
#include "SystemInfo.h"

namespace Cronos {

	//-------------------------------------------------------------------//
	//-------------------------- SOFTWARE INFO --------------------------//
	//-------------------------------------------------------------------//
	void SoftwareInfo::GetValues()
	{
		mSoftware_CppVersion	 =	ExtractCppVersion(__cplusplus);
		mSoftware_WindowsVersion =	ExtractWindowsVersion();
		mSoftware_SDLVersion	 =	ExtractSDLVersion();
	}


	const std::string SoftwareInfo::ExtractCppVersion(long int cppValue)
	{
		std::string tmp_cppVersion = "NULL: return value does not match with any C++ version!";
		switch (cppValue) {

			case(199711L):
				tmp_cppVersion = "C++ 98 or C++03";
				break;
			case(201103L):
				tmp_cppVersion = "C++11";
				break;
			case(201402L):
				tmp_cppVersion = "C++14";
				break;
			case(201703L):
				tmp_cppVersion = "C++17";
				break;
			default:
				tmp_cppVersion = "NULL: return value does not match with any C++ version!";
				break;
		}

		return tmp_cppVersion;
	}


	const std::string SoftwareInfo::ExtractWindowsVersion()
	{
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


	const std::string SoftwareInfo::ExtractSDLVersion()
	{
		SDL_version linked;
		SDL_version compiled;

		SDL_VERSION(&compiled);
		SDL_GetVersion(&linked);

		std::string VersionString = "SDL Compiled Version " + std::to_string(compiled.major)
			+ std::to_string(compiled.minor) + std::to_string(compiled.patch);

		VersionString += ("\nSDL Linked Version " + std::to_string(linked.major)
			+ std::to_string(linked.minor) + std::to_string(linked.patch));

		return VersionString;
	}


	const std::string SoftwareInfo::GetCppCompilerVersion()
	{
		return (std::to_string(_MSVC_LANG) + " (" + ExtractCppVersion(_MSVC_LANG) + ")");
	}


	//-------------------------------------------------------------------//
	//--------------------------- MEMORY INFO ---------------------------//
	//-------------------------------------------------------------------//
	void MemoryHardware::GetValues()
	{
		ExtractMemoryInfo();
	}

	
	void MemoryHardware::ExtractMemoryInfo() const
	{
		MEMORYSTATUSEX tmpMemoryInfo;
		tmpMemoryInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&tmpMemoryInfo);
		
		m_MemoryInfo = tmpMemoryInfo;
		m_MemoryInfo.dwLength = sizeof(MEMORYSTATUSEX);

		GetProcessMemoryInfo(GetCurrentProcess(), &m_ProcessMemCounters, sizeof(m_ProcessMemCounters));
		mProcess_vMemUsed = m_ProcessMemCounters.PagefileUsage;
		mProcess_physMemUsed = m_ProcessMemCounters.WorkingSetSize;

	}


	//-------------------------------------------------------------------//
	//---------------------------- GPU INFO -----------------------------//
	//-------------------------------------------------------------------//
	void GPUHardware::GetValues()
	{
		m_GPUCurrentVRAM = GetGPUCurrentVRAM();
		m_GPUTotalVRAM = GetGPUTotalVRAM();
	}

	
	const GLint GPUHardware::GetGPUTotalVRAM()
	{
		GLint tmp_GPUTotalVRAM = 0;
		//glGetIntegerv(0x9048, &tmp_GPUTotalVRAM);

		m_GPUCurrentVRAM = tmp_GPUTotalVRAM;
		return m_GPUTotalVRAM/KBTOMB;
	}


	const GLint GPUHardware::GetGPUCurrentVRAM()
	{
		GLint tmp_GPUCurrentVRAM = 0;
		//glGetIntegerv(0x9049, &tmp_GPUCurrentVRAM);

		m_GPUCurrentVRAM = tmp_GPUCurrentVRAM;
		return tmp_GPUCurrentVRAM/KBTOMB;
	}


	//-------------------------------------------------------------------//
	//--------------------------- SYSTEM INFO ---------------------------//
	//-------------------------------------------------------------------//
	void ProcessorHardware::GetValues()
	{
		GetCPUSystemInfo();
		//PrintCPUBrand();
		CheckForCPUInstructionsSet();
	}

	
	const std::string ProcessorHardware::ExtractCPUArchitecture(SYSTEM_INFO& SystemInfo)
	{
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


	void ProcessorHardware::PrintCPUBrand()
	{
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

		//App->EditorGUI->TestLog << " CPU: " << CPUBrandString << std::endl << std::endl;
	}


	void ProcessorHardware::GetCPUSystemInfo()
	{
		GetSystemInfo(&m_CpuSysInfo);
		m_CpuArchitecture = ExtractCPUArchitecture(m_CpuSysInfo);
	}


	void ProcessorHardware::CheckForCPUInstructionsSet()
	{
		if (SDL_Has3DNow() == true)
			m_CPUInstructionSet.Available_3DNow = true;
		if (SDL_HasRDTSC() == true)
			m_CPUInstructionSet.RDTSC_Available = true;
		if (SDL_HasAltiVec() == true)
			m_CPUInstructionSet.AltiVec_Available = true;
		if (SDL_HasAVX() == true)
			m_CPUInstructionSet.AVX_Available = true;
		if (SDL_HasAVX2() == true)
			m_CPUInstructionSet.AVX2_Available = true;
		if (SDL_HasMMX() == true)
			m_CPUInstructionSet.MMX_Available = true;
		if (SDL_HasSSE() == true)
			m_CPUInstructionSet.SSE_Available = true;
		if (SDL_HasSSE2() == true)
			m_CPUInstructionSet.SSE2_Available = true;
		if (SDL_HasSSE3() == true)
			m_CPUInstructionSet.SSE3_Available = true;
		if (SDL_HasSSE41() == true)
			m_CPUInstructionSet.SSE41_Available = true;
		if (SDL_HasSSE42() == true)
			m_CPUInstructionSet.SSE42_Available = true;
	}


	const std::string ProcessorHardware::GetCPUInstructionSet() const
	{
		std::string ret = "";
		InstructionsSet is = m_CPUInstructionSet;

		if (is.Available_3DNow == true)
			ret += "3DNOW, ";

		if (is.RDTSC_Available == true)
			ret += "RDTSC, ";

		if (is.AltiVec_Available == true)
			ret += "AltiVec, ";

		if (is.AVX_Available == true)
			ret += "AVX, ";

		if (is.AVX2_Available == true)
			ret += "AVX2, ";

		if (is.MMX_Available == true)
			ret += "MMX, ";

		if (is.SSE_Available == true)
			ret += "SSE, ";

		if (is.SSE2_Available == true)
			ret += "SSE2, ";

		if (is.SSE3_Available == true)
			ret += "SSE3, ";

		if (is.SSE41_Available == true)
			ret += "SSE41, ";

		if (is.SSE42_Available == true)
			ret += "SSE42, ";

		ret += '\n';
		return ret;
	}


	//-------------------------------------------------------------------//
	//--------------------------- SYSTEM INFO ---------------------------//
	//-------------------------------------------------------------------//
	SystemInfo::SystemInfo()
	{
		mSoftware_Info.GetValues();
		mHardware_MemoryInfo.GetValues();
		mHardware_GPUInfo.GetValues();
		mHardware_CPUInfo.GetValues();
	}


	SystemInfo::~SystemInfo()
	{
		//if (mSoftware_Info != nullptr)
		//{
		//	delete mSoftware_Info;
		//	mSoftware_Info = nullptr;
		//}
		//
		//if (mHardware_MemoryInfo != nullptr)
		//{
		//	delete mHardware_MemoryInfo;
		//	mHardware_MemoryInfo = nullptr;
		//}
		//
		//if (mHardware_CPUInfo != nullptr)
		//{
		//	delete mHardware_CPUInfo;
		//	mHardware_CPUInfo = nullptr;
		//}
		//
		//if (mHardware_GPUInfo != nullptr)
		//{
		//	delete mHardware_GPUInfo;
		//	mHardware_GPUInfo = nullptr;
		//}
	}

}