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
		virtual ~SDLWindow();

		virtual bool OnInit() override;
		virtual bool OnCleanUp() override;

		void ReCalculateAspectRatio(uint width, uint height);
		void OnResize(uint width, uint height, bool comingFromSceneFBO);

		//Save/Load
		virtual void SaveModuleData(json& JSONFile) const override;
		virtual void LoadModuleData(json& JSONFile) override;

	public:

		//The window we'll be rendering to
		SDL_Window* window = nullptr;

		//The surface contained by the window
		SDL_Surface* screen_surface = nullptr;

		//Getters
		const inline uint GetWidth()			const { return m_Data.Width;		}
		const inline uint GetHeight()			const { return m_Data.Height;		}
		const inline uint GetScreenSize()		const { return m_Data.ScreenSize;	}
		const inline float GetAspectRatio()		const { return m_Data.AspectRatio;	}
		uint cursorPositionX=0;
		uint cursorPositionY=0;
		//Setters
		void SetTitle(const char* title);
		//void SetAspectRatio(float AR) { m_Data.AspectRatio = AR;}
		//void SetWindowFullscreen(bool setStatus);
		//void SetWindowResizable(bool setStatus);
		//void SetWindowBorderless(bool setStatus);
		//void SetWindowDesktopFullscreen(bool setStatus);
		//void SetWindowBright(float brightValue);

	private:

		struct WindowData {

			//std::string Title;
			uint  Width, Height, ScreenSize;
			float AspectRatio = 1.0f;

			bool WindowFullscreen;
			bool WindowResizable;
			bool WindowBorderless;
			bool WindowDesktopFullscreen;
			float WindowBright;
		};

		WindowData m_Data;
		Uint32 m_WindowFlags;
	};

}

#endif
