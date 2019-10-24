#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Components/Component.h"

namespace Cronos {

	class Component;

	class GameObject
	{
	public:

		GameObject(std::string name,bool start_enabled = true);
		~GameObject();

		void Update();
		void Enable();
		void Disable();

		inline bool isActive() const { return m_Active; }
		void CreateComponent(ComponentType type);
		std::vector<Component*> GetComponents() { return m_Components; }

	private:

		bool m_Active;
		std::string m_Name;
		std::vector<Component*> m_Components;

	};

}

#endif // !_GAMEOBJECT_H_
