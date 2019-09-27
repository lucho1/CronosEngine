#include "cnpch.h"
#include "Globals.h"
#include "Application.h"
#include "SDLWindow.h"

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
			//Create window
			m_Data.Width = SCREEN_WIDTH * SCREEN_SIZE;
			m_Data.Height = SCREEN_HEIGHT * SCREEN_SIZE;
			m_Data.AspectRatio = (float)m_Data.Width / (float)m_Data.Height;
			Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

			//Use OpenGL 2.1
			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			//SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, Mv);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, mv);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

			if (WIN_FULLSCREEN == true)
			{
				flags |= SDL_WINDOW_FULLSCREEN;
			}

			if (WIN_RESIZABLE == true)
			{
				flags |= SDL_WINDOW_RESIZABLE;
			}

			if (WIN_BORDERLESS == true)
			{
				flags |= SDL_WINDOW_BORDERLESS;
			}

			if (WIN_FULLSCREEN_DESKTOP == true)
			{
				flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
			}

			window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_Data.Width, m_Data.Height, flags);

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
		}

		return ret;
	}


	bool SDLWindow::OnStart()
	{
		COMPILATIONLOGINFO;
		GL_SETERRORHANDLER(Mv, mv);

		return true;
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

	void SDLWindow::SetTitle(const char* title)
	{
		SDL_SetWindowTitle(window, title);
	}

	void SDLWindow::ReCalculateAspectRatio(uint width, uint height)
	{
		m_Data.AspectRatio = (float)width / (float)height;
	}

	void SDLWindow::OnResize(uint width, uint height)
	{
		glViewport(0, 0, width, height);
		ReCalculateAspectRatio(width, height);
		App->engineCamera->CalculateProjection();
	}
}