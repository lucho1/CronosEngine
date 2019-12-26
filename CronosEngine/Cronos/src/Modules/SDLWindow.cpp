#include "Providers/cnpch.h"

#include "Providers/Globals.h"
#include "Application.h"
#include "SDLWindow.h"
#include "EngineCamera.h"

#include "mmgr/mmgr.h"

namespace Cronos {

	SDLWindow::SDLWindow(Application* app, bool start_enabled) : Module(app, "Module SDL Window", start_enabled)
	{
		window = NULL;
		screen_surface = NULL;
	}

	// Destructor
	SDLWindow::~SDLWindow()
	{
	}

	// Called before render is available
	bool SDLWindow::OnInit()
	{
		LOG("Init SDL window & surface");
		bool ret = true;

		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			window = SDL_CreateWindow(App->GetAppTitle().c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_Data.Width, m_Data.Height, m_WindowFlags);

			if (window == NULL)
			{
				LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
				ret = false;
			}
			else
			{
				//Get window surface
				screen_surface = SDL_GetWindowSurface(window);
			}

			//SetWindowFullscreen(m_Data.WindowFullscreen);
			//SetWindowResizable(m_Data.WindowResizable);
			//SetWindowBorderless(m_Data.WindowBorderless);
			//SetWindowDesktopFullscreen(m_Data.WindowDesktopFullscreen);
			//SetWindowBright(m_Data.WindowBright);
		}

		return ret;
	}


	// Called before quitting
	bool SDLWindow::OnCleanUp()
	{
		LOG("Destroying SDL window and quitting all SDL systems");

		//Destroy window
		if (window != NULL)
		{
			SDL_DestroyWindow(window);
		}

		//Quit SDL subsystems
		SDL_Quit();
		return true;
	}


	void SDLWindow::SaveModuleData(json& JSONFile) const
	{
		JSONFile["Window"]["ScreenWidth"] = m_Data.Width;
		JSONFile["Window"]["ScreenHeight"] = m_Data.Height;
		JSONFile["Window"]["ScreenSize"] = m_Data.ScreenSize;

		JSONFile["Window"]["Fullscreen"] = m_Data.WindowFullscreen;
		JSONFile["Window"]["DesktopFullscreen"] = m_Data.WindowDesktopFullscreen;
		JSONFile["Window"]["Resizable"] = m_Data.WindowResizable;
		JSONFile["Window"]["Borderless"] = m_Data.WindowBorderless;

		JSONFile["Window"]["Bright"] = m_Data.WindowBright;
	}

	void SDLWindow::LoadModuleData(json& JSONFile)
	{
		m_Data.Width = JSONFile["Window"]["ScreenWidth"];
		m_Data.Height = JSONFile["Window"]["ScreenHeight"];
		m_Data.ScreenSize = JSONFile["Window"]["ScreenSize"];
		ReCalculateAspectRatio(m_Data.Width, m_Data.Height);

		m_Data.WindowFullscreen = JSONFile["Window"]["Fullscreen"];
		m_Data.WindowDesktopFullscreen = JSONFile["Window"]["DesktopFullscreen"];
		m_Data.WindowResizable = JSONFile["Window"]["Resizable"];
		m_Data.WindowBorderless = JSONFile["Window"]["Borderless"];

		m_Data.WindowBright = JSONFile["Window"]["Bright"];

		m_WindowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		if (m_Data.WindowResizable == true)
			m_WindowFlags |= SDL_WINDOW_RESIZABLE;

		//SetWindowFullscreen(m_Data.WindowFullscreen);
		//SetWindowResizable(m_Data.WindowResizable);
		//SetWindowBorderless(m_Data.WindowBorderless);
		//SetWindowDesktopFullscreen(m_Data.WindowDesktopFullscreen);
	}


	void SDLWindow::ReCalculateAspectRatio(uint width, uint height)
	{
		m_Data.AspectRatio = (float)width / (float)height;
	}


	void SDLWindow::OnResize(uint width, uint height)
	{
 		glViewport(0, 0, width, height);
		ReCalculateAspectRatio(width, height);
		App->engineCamera->ChangeProjection();
		m_Data.Width = width;
		m_Data.Height = height;
		//SetWindowFullscreen(false);
		//SetWindowDesktopFullscreen(false);
	}


	//void SDLWindow::SetWindowFullscreen(bool setStatus)
	//{
	//	m_Data.WindowFullscreen = setStatus;
	//	if (setStatus == true) {

	//		SetWindowDesktopFullscreen(false);
	//		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	//		m_WindowFlags |= SDL_WINDOW_FULLSCREEN;
	//	}
	//	else
	//		SDL_SetWindowFullscreen(window, 0);				

	//	OnResize(m_Data.Width, m_Data.Height);
	//}

	//void SDLWindow::SetWindowResizable(bool setStatus)
	//{
	//	//if (setStatus != m_Data.WindowResizable) {
	//	//
	//	//	m_Data.WindowResizable = setStatus;
	//	//	SDL_SetWindowResizable(window, (SDL_bool)setStatus);
	//	//
	//	//	if(setStatus == true)
	//	//		m_WindowFlags |= SDL_WINDOW_RESIZABLE;
	//	//}
	//}

	//void SDLWindow::SetWindowBorderless(bool setStatus)
	//{
	//	m_Data.WindowBorderless = setStatus;
	//	SDL_SetWindowBordered(window, (SDL_bool)!setStatus);

	//	if(setStatus == true)
	//		m_WindowFlags |= SDL_WINDOW_BORDERLESS;
	//}

	//void SDLWindow::SetWindowDesktopFullscreen(bool setStatus)
	//{
	//	m_Data.WindowDesktopFullscreen = setStatus;
	//	if(setStatus == true)
	//		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	//	else
	//		SDL_SetWindowFullscreen(window, 0);
	//	/*if (setStatus == true) {

	//		SetWindowFullscreen(false);
	//		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	//		m_WindowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	//	}
	//	else
	//		SDL_SetWindowFullscreen(window, 0);		*/	
	//}

	//void SDLWindow::SetWindowBright(float BrightValue)
	//{
	//	if (BrightValue < 0.0f || BrightValue > 100.0f) {
	//	
	//		CRONOS_WARN((BrightValue < 0.0f || BrightValue > 100.0f), "Couldn't change Brighteness. Bright Value must be between 0 and 100!!");
	//		return;
	//	}
	//	else
	//		BrightValue /= 100;

	//	m_Data.WindowBright = BrightValue;

	//	if (SDL_SetWindowBrightness(window, BrightValue) != 0)
	//		LOG("Could not change window brightness: %s\n", SDL_GetError());
	//}

	void SDLWindow::SetTitle(const char* title)
	{
		SDL_SetWindowTitle(window, title);
	}
}
