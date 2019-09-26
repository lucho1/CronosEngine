#ifndef _SDLWINDOW_H_
#define _SDLWINDOW_H_

#include "Module.h"
#include "SDL/include/SDL.h"

namespace Cronos {

	class Application;

	class SDLWindow : public Module
	{
	public:

		friend class ImGuiLayer;

		SDLWindow(Application* app, bool start_enabled = true);

		// Destructor
		virtual ~SDLWindow();

		virtual bool OnInit() override;
		virtual bool OnStart() override;
		virtual bool OnCleanUp() override;

		void SetTitle(const char* title);

	public:
		//The window we'll be rendering to
		SDL_Window* window;

		//The surface contained by the window
		SDL_Surface* screen_surface;
		
		inline uint GetWidth() const { return m_Data.Width; }
		inline uint GetHeight() const { return m_Data.Height; }
		

	public:

		struct WindowData {

			std::string Title;
			uint  Width, Height;
			bool VSync;
			
		};

		WindowData m_Data;
		int Mv = 4, mv = 3; //Open GL Major and Minor version
	};

}

#endif