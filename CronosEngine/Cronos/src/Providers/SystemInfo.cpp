#include "Providers/cnpch.h"
#include "SystemInfo.h"

namespace Cronos {

	//-------------------------------------------------------------------//
	//-------------------------- SOFTWARE INFO --------------------------//
	//-------------------------------------------------------------------//
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


	const std::string SoftwareInfo::GetCppCompilerVersion()
	{
		return (std::to_string(_MSVC_LANG) + " (" + ExtractCppVersion(_MSVC_LANG) + ")");
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


	//-------------------------------------------------------------------//
	//--------------------------- MEMORY INFO ---------------------------//
	//-------------------------------------------------------------------//
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
	const GLint GPUHardware::GetGPUTotalVRAM()
	{
		glGetIntegerv(0x9048, &m_GPUTotalVRAM);
		return m_GPUTotalVRAM/KBTOMB;
	}

	const GLint GPUHardware::GetGPUCurrentVRAM()
	{
		glGetIntegerv(0x9049, &m_GPUCurrentVRAM);
		return m_GPUCurrentVRAM/KBTOMB;
	}


	//-------------------------------------------------------------------//
	//--------------------------- SYSTEM INFO ---------------------------//
	//-------------------------------------------------------------------//
	bool SystemInfo::OnInit()
	{
		//mSoftware_Info->mSoftware_CppVersion = mSoftware_Info->ExtractCppVersion(2);
		//mSoftware_Info->mSoftware_WindowsVersion = mSoftware_Info->ExtractWindowsVersion();
		//mSoftware_Info->mSoftware_SDLVersion = mSoftware_Info->ExtractSDLVersion();
		//mHardware_MemoryInfo->ExtractMemoryInfo();
	}



}