#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Components/Component.h"
#include "glm/glm.hpp"

namespace Cronos {

	class Component;

	class GameObject
	{
		friend class PrimitiveGameObject;
	public:
		
		GameObject(const std::string& name, int gameObjectID, const std::string& path, bool start_enabled = true, glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f), 
			glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));

		~GameObject();

		virtual void Update(float dt);
		void Enable();
		void Disable();
		void CleanUp();

		inline bool isActive() const { return m_Active; }
		bool &SetActive() { return m_Active; }

		inline const std::string GetName() const { return m_Name; }
		inline const std::string GetPath() const { return m_Path; }
		void SetName(const std::string name) { m_Name = name; }
		void SetPath(const std::string path) { m_Path = path; }
		
		Component* CreateComponent(ComponentType type);
		std::list<GameObject*> m_Childs;
		std::vector<Component*> m_Components;

		template <typename T>
		T* GetComponent()
		{
			ComponentType type = T::GetType();
			for (auto& comp : m_Components)
				if (comp->GetComponentType() == type)
					return ((T*)(comp));
			
			LOG("Component %i in %s Game Object NOT Found!", (int)type, m_Name.c_str());
			return nullptr;
		}

	public:

		bool m_IsPrimitive = false;

	private:

		std::string m_Name;
		std::string m_Path;
		bool m_Active;
		int m_GameObjectID;
	};

}

#endif // !_GAMEOBJECT_H_
