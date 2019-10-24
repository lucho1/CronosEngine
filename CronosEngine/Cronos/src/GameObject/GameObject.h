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
		Component* CreateComponent(ComponentType type);

		template <typename T>
		T* GetComponent()
		{
			T::GetType()//TODO do it on every class 
			for (auto& comp : m_Components) {
				if (comp->GetComponentType == type)
					return ((T*)(comp));

				
			}
			return nullptr;
		}

	private:

		bool m_Active;
		std::string m_Name;
		ComponentTransform* a;
		std::vector<Component*> m_Components;
		
		std::list<GameObject*> m_Childs;
	};

}

#endif // !_GAMEOBJECT_H_
