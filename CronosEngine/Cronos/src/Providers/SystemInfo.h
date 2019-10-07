#ifndef _SYSTEMINFO_H_
#define _SYSTEMINFO_H_

#include "psapi.h"

#define BTOGB (1073741824.0f)
#define KBTOMB 1024.0f //To GB: (1048576.0f)
#define BTOMB (1048576.0f)


namespace Cronos
{

	//-------------------------- SOFTWARE INFO --------------------------//
	class SoftwareInfo
	{
	private:

		//Don't use this, they are just class variables, use the getters instead
		mutable std::string mSoftware_CppVersion;
		mutable std::string mSoftware_WindowsVersion;
		mutable std::string mSoftware_SDLVersion;

	private:

		//Methods to check the versions for different software -- DON'T USE THEM!
		const std::string ExtractCppVersion(long int cppValue);
		const std::string ExtractWindowsVersion();
		const std::string ExtractSDLVersion();

	public:

		void GetValues(); //DON'T USE THIS FUNCTION, IS JUST FOR CLASS PURPOSES!!!

		//Methods to return the different values for software versions... Ready to print -- Use them :)
		const std::string GetWindowsVersion()	const		{ return mSoftware_WindowsVersion; }
		const std::string OsFoundString()		const		{ return (__STDC_HOSTED__ ? "OS Found" : "OS NOT FOUND!"); }
											
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


	//--------------------------- MEMORY INFO ---------------------------//
	//TODO: FALTA LO DE RICK
	class MemoryHardware
	{
	private:

		mutable MEMORYSTATUSEX m_MemoryInfo;

		mutable PROCESS_MEMORY_COUNTERS m_ProcessMemCounters;
		mutable SIZE_T mProcess_vMemUsed;
		mutable SIZE_T mProcess_physMemUsed;

		void ExtractMemoryInfo()					const;

	public:

		void GetValues(); //DON'T USE THIS FUNCTION, IS JUST FOR CLASS PURPOSES!!!

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
	
		const uint GetVirtualMemoryUsedByProcess()	const	{ return(uint)mProcess_vMemUsed/BTOMB; } //In MB
		const uint GetPhysMemoryUsedByProcess()		const	{ return(uint)mProcess_physMemUsed/BTOMB; } //In MB
	
	};


	//---------------------------- GPU INFO -----------------------------//
	class GPUHardware
	{
	private:

		struct GPUPrimaryInfo_IntelGPUDetect
		{
			std::string m_GPUBrand;
			uint m_GPUID = 0;
			uint m_GPUVendor = 0;

			//'PI' is for "Primary Info", 'GPUDet' is for "GPUDetect", the code we use to get this info
			//Info in Bytes
			uint64 mPI_GPUDet_TotalVRAM_Bytes = 0;
			uint64 mPI_GPUDet_VRAMUsage_Bytes = 0;
			uint64 mPI_GPUDet_CurrentVRAM_Bytes = 0;
			uint64 mPI_GPUDet_VRAMReserved_Bytes = 0;
			
			//Info in MB (Just change the division of BTOMB in the GPUDetect_ExtractGPUInfo() function
			//by the BTOGB one to get the GB). And change this variables name!! (_MB change it by _GB!!!)
			float mPI_GPUDet_TotalVRAM_MB = 0.0f;
			float mPI_GPUDet_VRAMUsage_MB = 0.0f;
			float mPI_GPUDet_CurrentVRAM_MB = 0.0f;
			float mPI_GPUDet_VRAMReserved_MB = 0.0f;
		
		};

		mutable GPUPrimaryInfo_IntelGPUDetect m_PI_GPUDet_GPUInfo; //This is the Info for the current GPU in use

		//For OPENGL GPU "Getter"
		GLint m_GPUTotalVRAM = 0;
		GLint m_GPUCurrentVRAM = 0;

	private:

		void GPUDetect_ExtractGPUInfo() const;

	public:

		void GetValues();

		const auto GetGPUBenchmark()	const { return glGetString(GL_VENDOR); }
		const auto GetGPUModel()		const { return glGetString(GL_RENDERER); }

		const GLint GetGPUTotalVRAM();  // In MB... Only for NVIDIA GPUs, otherwise returns 0
		const GLint GetGPUCurrentVRAM(); // In MB... Only for NVIDIA GPUs, otherwise returns 0
		
		const GPUPrimaryInfo_IntelGPUDetect GetGPUInfo_GPUDet() const { return m_PI_GPUDet_GPUInfo; }	
	};


	//---------------------------- CPU INFO -----------------------------//
	class ProcessorHardware
	{
	private:

		SYSTEM_INFO m_CpuSysInfo;
		std::string m_CPUBrand;
		std::string m_CPUVendor;

		std::string m_CpuArchitecture;

		struct InstructionsSet
		{
			bool Available_3DNow = false;
			bool RDTSC_Available = false;
			bool AltiVec_Available = false;
			bool AVX_Available = false;
			bool AVX2_Available = false;
			bool MMX_Available = false;
			bool SSE_Available = false;
			bool SSE2_Available = false;
			bool SSE3_Available = false;
			bool SSE41_Available = false;
			bool SSE42_Available = false;
		};

		InstructionsSet m_CPUInstructionSet;

	private:

		void GetCPUSystemInfo();
		const std::string ExtractCPUArchitecture(SYSTEM_INFO& SystemInfo);
		void CheckForCPUInstructionsSet();

	public:

		void GetValues();

		const uint GetCPUCores()						const	{ return SDL_GetCPUCount(); }
		const uint GetCPUCacheLine1Size()				const	{ return SDL_GetCPUCacheLineSize(); } //In bytes

		const std::string GetCPUArchitecture()			const	{ return m_CpuArchitecture; }
		const std::string GetNumberOfProcessors()		const	{ return std::to_string(m_CpuSysInfo.dwNumberOfProcessors); }
		const std::string GetProcessorRevision()		const	{ return std::to_string(m_CpuSysInfo.wProcessorRevision); }

		const InstructionsSet GetCPUInstructionsSet()	const	{ return m_CPUInstructionSet; }
		const std::string GetCPUInstructionSet()		const;

		const std::string GetCPUBrand()					const { return m_CPUBrand; }
		const std::string GetCPUVendor()				const { return m_CPUVendor; }
	};


	//--------------------------- SYSTEM INFO ---------------------------//
	//This will handle all the hardware and software info classes to have
	//it all unified
	class SystemInfo
	{
	private:

		SoftwareInfo mSoftware_Info;
		MemoryHardware mHardware_MemoryInfo;
		GPUHardware mHardware_GPUInfo;
		ProcessorHardware mHardware_CPUInfo;

	public:

		SystemInfo();
		~SystemInfo() {}

		const SoftwareInfo GetSoftwareInfo()			const { return mSoftware_Info; }
		const MemoryHardware GetMemoryHardwareInfo()	const { return mHardware_MemoryInfo; }
		const ProcessorHardware GetCPUHardwareInfo()	const { return mHardware_CPUInfo; }
		const GPUHardware GetGPUHardwareInfo()			const { return mHardware_GPUInfo; }

	};
	
}
#endif