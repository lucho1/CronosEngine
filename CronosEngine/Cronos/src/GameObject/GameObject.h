#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Components/Component.h"
#include "Components/ComponentTransform.h"
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
		inline std::string GetName() const { return m_Name; }
		void SetName(const std::string name) { m_Name = name; }
		bool &setActive() { return m_Active; }
		Component* CreateComponent(ComponentType type);
		std::list<GameObject*> m_Childs;

		template <typename T>
		T* GetComponent()
		{
			ComponentType type = T::GetType();
			for (auto& comp : m_Components) {
				if (comp->GetComponentType() == type)
					return ((T*)(comp));			
			}
			return nullptr;
		}

	private:

		bool m_Active;
		std::string m_Name;
		std::vector<Component*> m_Components;

	};

}

#endif // !_GAMEOBJECT_H_
