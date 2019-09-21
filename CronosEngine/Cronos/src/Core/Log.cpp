#pragma once

#include "cnpch.h"
#include "Globals.h"
#include "Application.h"

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

	void LogCompileFirstMessage()
	{
		std::cout << "-- Compilation Start Succeeded --" << std::endl << "	Compilation at Date " << __DATE__ << " and Time " << __TIME__ << std::endl << std::endl << std::endl;

		//__cplusplus returning values:
		/*	199711L (C++98 or C++03)
			201103L (C++11)
			201402L (C++14)
			201703L (C++17)
		*/
		std::string cppVersion = "NULL";
		switch (__cplusplus) {

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
			cppVersion = "__cplusplus macro return value does not match with any C++ version!";
			break;
		}

		std::cout << "Standatd C++ Version Implemented by Compiler: " << __cplusplus << " (" << cppVersion << ")" << std::endl <<
			"OS Found = " << (__STDC_HOSTED__ ? "True" : "False") << std::endl << std::endl << std::endl;
		std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
		std::cout << "GPU Benchmark: " << glGetString(GL_VENDOR) << std::endl <<
			"	GPU: " << glGetString(GL_RENDERER) << std::endl << std::endl <<
			"OpenGL Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl << std::endl << std::endl;
	}

}