#ifndef _SYSTEMINFO_H_
#define _SYSTEMINFO_H_

#include "Module.h"

#include "Application.h"
#include "Providers/Globals.h"

namespace Cronos
{

	class SystemInfo : public Module
	{

	private:

		SoftwareInfo* mSoftware_Info = nullptr;
		
	public:

		virtual bool OnInit() override;
		virtual bool OnCleanUp() override;
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


		const std::string GetWindowsVersion()		{ return mSoftware_WindowsVersion; }
		const std::string OsFound()					{ return (__STDC_HOSTED__ ? "OS Found" : "OS NOT FOUND!"); }

		const std::string GetSDLVersion()			{ return mSoftware_SDLVersion; }
		const std::string GetOGLVersion()			{ return glGetString(GL_VERSION); }
		
		
		///__cplusplus returning values:
		///199711L (C++98 or C++03)
		///201103L (C++11)
		///201402L (C++14)
		///201703L (C++17)
		const std::string GetCppVersion()			{ return mSoftware_CppVersion; }
		const std::string GetCPPLongVersion()		{ return std::to_string(__cplusplus); }

		const std::string GetCompilationDate()		{ return __DATE__; }
		const std::string GetCompilationTime()		{ return __TIME__; }
	};


}

#endif