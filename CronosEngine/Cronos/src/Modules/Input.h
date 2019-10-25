#ifndef _INPUT_H_
#define _INPUT_H_

#include "Module.h"
#include "Providers/Globals.h"

namespace Cronos {

#define MAX_MOUSE_BUTTONS 5

	enum KEY_STATE
	{
		KEY_IDLE = 0,
		KEY_DOWN,
		KEY_REPEAT,
		KEY_UP
	};

	class Input : public Module
	{
	public:

		Input(Application* app, bool start_enabled = true);
		~Input();

		virtual bool OnInit() override;
		virtual update_status OnPreUpdate(float dt) override;
		virtual bool OnCleanUp() override;


		KEY_STATE GetKey(int id) const
		{
			return keyboard[id];
		}

		KEY_STATE GetMouseButton(int id) const
		{
			return mouse_buttons[id];
		}
		inline bool isMouseScrolling() const
		{
			return m_MouseScroll;
		}
		inline int GetMouseX() const
		{
			return mouse_x;
		}

		inline int GetMouseY() const
		{
			return mouse_y;
		}

		inline int GetMouseZ() const
		{
			return mouse_z;
		}

		inline int GetMouseXMotion() const
		{
			return mouse_x_motion;
		}

		inline int GetMouseYMotion() const
		{
			return mouse_y_motion;
		}

		inline const bool getCurrentWinStatus() { return quit; }
		inline void updateQuit(bool updated) { quit = updated; }
	private:

		KEY_STATE* keyboard = nullptr;
		KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
		int mouse_x;
		int mouse_y;
		int mouse_z;
		int mouse_x_motion;
		int mouse_y_motion;
		bool quit = false;
		bool m_MouseScroll;

	};

}

#endif
