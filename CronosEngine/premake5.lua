workspace "Cronos"
	architecture "x86"
	--startproject "CronosExtern"

	configurations
	{
		"Debug",
		"Release"
	}

	flags --TODO: Wtf?? xddd
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["Glut"] = "Cronos/vendor/glut"
IncludeDir["SDL"] = "Cronos/vendor/SDL/include"
IncludeDir["SDL_Mixer"] = "Cronos/vendor/SDL_mixer/include"
IncludeDir["dlls"] = "Cronos/dll_files"
IncludeDir["Glad"] = "Cronos/vendor/Glad/include"
IncludeDir["ImGui"] = "Cronos/vendor/imgui"
IncludeDir["glm"] = "Cronos/vendor/glm"
IncludeDir["Assimp"]="Cronos/vendor/Assimp"
IncludeDir["gpuDetect"]="Cronos/vendor/gpudetect"

include "Cronos/vendor/Glad"
include "Cronos/vendor/imgui"

--PATH=%PATH%;$(ProjectDir)\dll_files
--Cronos Project (Engine)
project "Cronos"
	location "Cronos"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir("bin/" .. outputdir .. "/Cronos")
	objdir("bin-int/" .. outputdir .. "/Cronos")

	pchheader "Providers/cnpch.h"
	pchsource "Cronos/src/Providers/cnpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/glm/**.hpp",
		"%{prj.name}/vendor/glm/glm/**.inl",
		"%{prj.name}/vendor/gpudetect/**.cpp",
		"%{prj.name}/vendor/gpudetect/**.h",
		"%{prj.name}/vendor/mmgr/**.cpp",
		"%{prj.name}/vendor/mmgr/**.h"
	}

	defines
	{
	--	"_CRT_SECURE_NO_WARNINGS"
	--IMGUI_IMPL_OPENGL_LOADER_GLAD
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor",
		"%{prj.name}/vendor/SDL_mixer/libx86",
		"%{prj.name}/dll_files",
		"%{prj.name}/src/Core",
		"%{prj.name}/src/Helpers",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.Assimp}"
	}
	libdirs
	{
		"%{prj.name}/vendor/SDL_mixer/libx86",
		"%{prj.name}/vendor/SDL/libx86",
		"%{prj.name}/vendor/glut",
		"%{prj.name}/vendor/Assimp/libx86"
	}
	links
	{
		"glut32.lib",
		"opengl32.lib",
		"SDL2_mixer.lib",
		"SDL2.lib",
		"SDL2main.lib",
		"ImGui",
		"Glad",
		"Assimp.lib"
	}

	filter "system:windows"
		systemversion "latest" -- You can add here your windows SDK version if something fails

		linkoptions
		{
			"/SAFESEH:NO", -- Image Has Safe Exception Handers: No"
		}

		--For VS post build command (in project properties) this would be: xcopy "$(SolutionDir)Cronos\*.dll" "$(TargetDir)" /Y
		postbuildcommands
		{
			("{COPY}/dll_files/*.dll ../bin/" .. outputdir .. "/Cronos")
		}

	filter "configurations:Debug"
		defines "CN_DEBUG"
		runtime "Debug"
		symbols "on"
	filter "configurations:Release"
		defines "CN_RELEASE"
		runtime "Release"
		optimize "on"
