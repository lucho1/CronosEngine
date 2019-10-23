#pragma once

#include "Providers/cnpch.h"

#include "Providers/Globals.h"
#include "Application.h"

#include "../ImGui/ImGuiLayer.h"

#include "mmgr/mmgr.h"


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


	void LogCompilationFirstMessage()
	{
		App->EditorGUI->TestLog <<	"-- Compilation Start Succeeded --"	<<		std::endl			<<
									"	Compilation at Date "			<<		__DATE__			<<
									" and Time "						<<		__TIME__			<<
		std::endl;
	}
}
