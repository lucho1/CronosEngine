#include "Providers/cnpch.h"

#include "Providers/Globals.h"
#include "Application.h"

#include "ImGui/ImGuiLayer.h"
#include "ImGui/OpenGL/imgui_impl_sdl.h"

#include "Input.h"
#include "SDLWindow.h"
#include "Scene.h"

#include "mmgr/mmgr.h"

namespace Cronos {

#define MAX_KEYS 300

	Input::Input(Application* app, bool start_enabled) : Module(app, "Module Input", start_enabled)
	{
		keyboard = new KEY_STATE[MAX_KEYS];
		memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	}

	// Destructor
	Input::~Input()
	{
		delete[] keyboard;
	}

	// Called before render is available
	bool Input::OnInit()
	{
		LOG("Init SDL input event system");
		bool ret = true;
		SDL_Init(0);

		if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
		{
			LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}

		return ret;
	}

	// Called every draw update
	update_status Input::OnPreUpdate(float dt)
	{
		SDL_PumpEvents();

		const Uint8* keys = SDL_GetKeyboardState(NULL);

		for (int i = 0; i < MAX_KEYS; ++i)
		{
			if (keys[i] == 1)
			{
				if (keyboard[i] == KEY_IDLE) {
					keyboard[i] = KEY_DOWN;
					App->EditorGUI->GetInput(i, KEY_DOWN);
				}
				else if (keyboard[i] != KEY_REPEAT)
				{
					keyboard[i] = KEY_REPEAT;
					App->EditorGUI->GetInput(i, KEY_REPEAT);
				}
			}
			else
			{
				if (keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN) {
					keyboard[i] = KEY_UP;
					App->EditorGUI->GetInput(i, KEY_DOWN);
				}
				else
					keyboard[i] = KEY_IDLE;
			}
		}

		Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

		mouse_x /= App->window->GetScreenSize();
		mouse_y /= App->window->GetScreenSize();
		mouse_z = 0;

		for (int i = 0; i < 5; ++i)
		{
			if (buttons & SDL_BUTTON(i))
			{
				if (mouse_buttons[i] == KEY_IDLE) {
					mouse_buttons[i] = KEY_DOWN;
					App->EditorGUI->GetInput(i, KEY_DOWN,true);
				}
				else if(mouse_buttons[i]!=KEY_REPEAT){
					mouse_buttons[i] = KEY_REPEAT;
					App->EditorGUI->GetInput(i, KEY_REPEAT, true);
				}
			}
			else
			{
				if (mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN) {
					mouse_buttons[i] = KEY_UP;
					App->EditorGUI->GetInput(i, KEY_UP,true);
				}
				else
					mouse_buttons[i] = KEY_IDLE;
			}
		}

		mouse_x_motion = mouse_y_motion = 0;
		m_MouseScroll = false;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			ImGui_ImplSDL2_ProcessEvent(&e);
			switch (e.type)
			{
			case SDL_MOUSEWHEEL:
				m_MouseScroll = true;
				mouse_z = e.wheel.y;
				break;

			case SDL_MOUSEMOTION:
				mouse_x = e.motion.x / App->window->GetScreenSize();
				mouse_y = e.motion.y / App->window->GetScreenSize();

				mouse_x_motion = e.motion.xrel / App->window->GetScreenSize();
				mouse_y_motion = e.motion.yrel / App->window->GetScreenSize();
				break;

			case SDL_QUIT:
				quit = true;
				break;
			case SDL_DROPFILE: 
			{
				AssetItems* ItemTemp = new AssetItems(e.drop.file,nullptr);
				if (ItemTemp->GetType() == ItemType::ITEM_FBX || ItemTemp->GetType() == ItemType::ITEM_OBJ) {
					GameObject* goTemp = App->scene->CreateModel(ItemTemp->GetAbsolutePath().c_str());
					App->scene->m_GameObjects.push_back(goTemp);
					//App->EditorGUI->m_CurrentDir->m_Container.push_back(ItemTemp);

					std::string SrcPath = ItemTemp->GetAbsolutePath().c_str();
					std::string DestPath = App->EditorGUI->m_CurrentDir->m_LabelDirectories + "\\" + ItemTemp->m_AssetFullName;				
					std::wstring WSrcPath = std::wstring(SrcPath.begin(), SrcPath.end());
					std::wstring WDestPath = std::wstring(DestPath.begin(), DestPath.end());

					CopyFile(WSrcPath.c_str(), WDestPath.c_str(),0);
					std::string TempDestinationPath = DestPath.c_str();
					AssetItems* ItemRet = new AssetItems(TempDestinationPath.c_str(), App->EditorGUI->m_CurrentDir);
					App->EditorGUI->m_CurrentDir->m_Container.push_back(ItemRet);
					delete ItemTemp;
				}
				break;
			}
			case SDL_WINDOWEVENT:
			{
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
					App->window->OnResize(e.window.data1, e.window.data2, false);
			}
			}
		}

		//if (quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		//	return UPDATE_STOP;

		return UPDATE_CONTINUE;
	}

	// Called before quitting
	bool Input::OnCleanUp()
	{
		LOG("Quitting SDL input event subsystem");
		SDL_QuitSubSystem(SDL_INIT_EVENTS);
		return true;
	}

}
