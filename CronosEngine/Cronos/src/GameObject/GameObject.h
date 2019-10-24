#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Components/Component.h"
#include "glm/glm.hpp"

namespace Cronos {

	class Component;

	class GameObject
	{
	public:
		
		GameObject(std::string name,bool start_enabled = true,glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f), 
			glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));

		~GameObject();

		void Update(float dt);
		void Enable();
		void Disable();

		inline bool isActive() const { return m_Active; }
		Component* CreateComponent(ComponentType type);
		std::vector<Component*> GetComponents() { return m_Components; }

	private:

		bool m_Active;
		std::string m_Name;
		std::vector<Component*> m_Components;
		std::list<GameObject*> m_Childs;
	};

}

#endif // !_GAMEOBJECT_H_
