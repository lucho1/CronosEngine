#ifndef _SYSTEMINFO_H_
#define _SYSTEMINFO_H_

#include "Module.h"

#include "Application.h"
#include "Providers/Globals.h"

#include "psapi.h"

#define BTOGB (1073741824.0f)
#define KBTOMB 1024.0f//To GB: (1048576.0f)
#define BTOMB (1048576.0f)

namespace Cronos
{

	class SystemInfo : public Module
	{

	private:

		SystemInfo();
		~SystemInfo();

		SoftwareInfo*		mSoftware_Info = nullptr;
		MemoryHardware*		mHardware_MemoryInfo = nullptr;
		ProcessorHardware*	mHardware_CPUInfo = nullptr;
		GPUHardware*		mHardware_GPUInfo = nullptr;

	public:

		virtual bool OnInit() override;
		virtual bool OnCleanUp() override;
		
		const SoftwareInfo GetSoftwareInfo()			const { return *mSoftware_Info; }
		const MemoryHardware GetMemoryHardwareInfo()	const { return *mHardware_MemoryInfo; }
		const ProcessorHardware GetCPUHardwareInfo()	const { return *mHardware_CPUInfo; }
		const GPUHardware GetGPUHardwareInfo()			const { return *mHardware_GPUInfo; }
	
	};


	class SoftwareInfo
	{

	private:

		friend class SystemInfo;

		mutable std::string mSoftware_CppVersion;
		mutable std::string mSoftware_WindowsVersion;
		mutable std::string mSoftware_SDLVersion;

	private:

		const std::string ExtractCppVersion(long int cppValue);
		const std::string ExtractWindowsVersion();
		const std::string ExtractSDLVersion();


		const std::string GetWindowsVersion()	const		{ return mSoftware_WindowsVersion; }
		const std::string OsFound()				const		{ return (__STDC_HOSTED__ ? "OS Found" : "OS NOT FOUND!"); }
											
		const std::string GetSDLVersion()		const		{ return mSoftware_SDLVersion; }
		const auto GetOGLVersion()				const		{ return glGetString(GL_VERSION); }
		const auto GetOGLShadingVersion()		const		{ return glGetString(GL_SHADING_LANGUAGE_VERSION); }
		
		///__cplusplus returning values:
		///199711L (C++98 or C++03)
		///201103L (C++11)
		///201402L (C++14)
		///201703L (C++17)
		const std::string GetCppVersionImplementedByCompiler()	const	{ return mSoftware_CppVersion; }
		const std::string GetCPPNumericalVersion()				const	{ return std::to_string(__cplusplus); }
		const std::string GetCppCompilerVersion();

		const std::string GetVSCompilerVersion()				const	{ return (std::to_string(_MSC_VER)); }
		const std::string MultithreadedSpecified()				const	{ return std::string((_MT ? "" : "Not ")) + " Multithreaded Specified"; }

		const std::string GetCompilationDate()					const	{ return __DATE__; }
		const std::string GetCompilationTime()					const	{ return __TIME__; }
	
	};



	//TODO: FALTA LO DE RICK
	class MemoryHardware
	{
	private:

		friend class SystemInfo;

		mutable MEMORYSTATUSEX m_MemoryInfo;

		mutable PROCESS_MEMORY_COUNTERS m_ProcessMemCounters;
		mutable SIZE_T mProcess_vMemUsed;
		mutable SIZE_T mProcess_physMemUsed;

	private:

		void ExtractMemoryInfo()					const;

		const float GetRAMSizeFromSDL()				const	{ return (float)SDL_GetSystemRAM() / KBTOMB; } //In GB
		const uint32 GetRAMSize()					const	{ return (uint32)m_MemoryInfo.dwLength; } //In GB
		const uint32 GetPercentageOfMemoryLoad()	const	{ return (uint32)m_MemoryInfo.dwMemoryLoad; } //In %
		
		const uint64 GetPhysicalMemory()			const	{ return (uint64)m_MemoryInfo.ullTotalPhys/BTOGB; } //In GB
		const uint64 GetFreePhysicalMemory()		const	{ return (uint64)m_MemoryInfo.ullAvailPhys/BTOGB; } //In GB
		const uint64 GetUsedPhysicalMemory()		const	{ return (uint64)((m_MemoryInfo.ullTotalPhys - m_MemoryInfo.ullAvailPhys)/BTOGB); } //In GB
		
		const uint64 GetVirtualMemory()				const	{ return (uint64)m_MemoryInfo.ullTotalVirtual/BTOGB; } //In GB
		const uint64 GetFreeVirtualMemory()			const	{ return (uint64)m_MemoryInfo.ullAvailVirtual/BTOGB; } //In GB
		
		const uint64 GetFreeExtendedMemory()		const	{ return (uint64)m_MemoryInfo.ullAvailExtendedVirtual/BTOMB; } //In MB
		
		const uint64 GetPageFileMemory()			const	{ return (uint64)m_MemoryInfo.ullTotalPageFile / BTOGB; } //In GB
		const uint64 GetFreePageFileMemory()		const	{ return (uint64)m_MemoryInfo.ullAvailPageFile / BTOGB; } //In GB
	
		const uint GetMemoryUsedByProcess() const  { return(uint)mProcess_vMemUsed/BTOMB; } //In MB
		const uint GetMemoryUsedByProcess() const  { return(uint)mProcess_physMemUsed/BTOMB; } //In MB
	
	};


	class ProcessorHardware
	{

	private:

		friend class SystemInfo;
		SYSTEM_INFO m_CpuSysInfo;

		struct InstructionsSet
		{
			bool RDTSC_Available;

		};

	private:

		const std::string GetCPUArchitecture(SYSTEM_INFO& SystemInfo);
		const std::string GetCPUBrand();

		const uint GetCPUCores() const { return SDL_GetCPUCount(); }
		const uint GetCPUCacheLine1Size() const { return SDL_GetCPUCacheLineSize(); } //In bytes

		void CheckForCPUInstructionsSet();
	};


	class GPUHardware
	{

	private:

		friend class SystemInfo;

		GLint m_GPUTotalVRAM = 0;
		GLint m_GPUCurrentVRAM = 0;

	private:

		const auto GetGPUBenchmark()	const		{ return glGetString(GL_VENDOR); }
		const auto GetGPUModel()		const		{ return glGetString(GL_RENDERER); }

		const GLint GetGPUTotalVRAM();  // In MB... Only for NVIDIA GPUs, otherwise returns 0
		const GLint GetGPUCurrentVRAM(); // In MB... Only for NVIDIA GPUs, otherwise returns 0
	};

}

#endif