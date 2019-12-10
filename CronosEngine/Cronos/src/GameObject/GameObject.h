#ifndef _GAMEOBJECT_H_
#define _GAMEOBJECT_H_

#include "Components/Component.h"
#include "Components/MeshComponent.h"
#include "glm/glm.hpp"

namespace Cronos {

	class Component;

	class GameObject
	{
		friend class PrimitiveGameObject;
	public:

		//Game Object Methods
		GameObject(const std::string& name, int gameObjectID, const std::string& path, bool start_enabled = true, glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f),
			glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f));

		~GameObject();

		virtual void Update(float dt);
		void Enable();
		void Disable();
		void CleanUp();

		inline bool isActive() const { return m_Active; }
		bool &SetActive() { return m_Active; }

	public:

		//Getters
		inline const std::string GetName()			const { return m_Name; }
		inline const std::string GetPath()			const { return m_Path; }
		inline const std::string GetMetaPath()		const { return m_MetaPath; }

		inline const int GetGOID()					const { return m_GameObjectID; }
		inline const int GetCountChilds()			const { return m_Childs.size(); }

		GameObject* GetParentGameObject()			{ return Parent; }

		const math::AABB GetAABB()					const { return m_AABB; }
		const math::OBB GetOOBB()					const { return m_OBB; }

	public:

		//Setters
		void SetNewID();
		
		void SetName(const std::string name)		{ m_Name = name; }
		void SetPath(const std::string path)		{ m_Path = path; }
		void SetMeta(const std::string meta)		{ m_MetaPath = m_Path+meta; }
		void SetParent(GameObject* Go)				{ Parent = Go; }
		
		void SetAABB(math::AABB aabb)				{ m_AABB = aabb; }
		void SetOOBB(math::OBB oobb)				{ m_OBB = oobb; }
		void SetOOBBTransform(glm::vec3 translation, glm::quat orientation, glm::vec3 scale);

	public:
		
		//Others
		void BreakParent() { Parent = nullptr; }
		Component* CreateComponent(ComponentType type);

		template <typename T>
		T* GetComponent()
		{
			ComponentType type = T::GetType();
			for (auto& comp : m_Components)
				if (comp->GetComponentType() == type)
					return ((T*)(comp));

			return nullptr;
		}

		bool HasMeta()
		{
			if (std::filesystem::exists(m_MetaPath))
				return true;
			return false;
		}

	public:

		std::list<GameObject*> m_Childs;
		std::vector<Component*> m_Components;

		bool m_IsPrimitive = false;
		bool tempHasMaterial = false;
		bool HasMetaa = false;
		bool HasVertices = false;

		glm::vec3 m_AABBTranslation = glm::vec3(0.0f);

	private:

		GameObject* Parent = nullptr;
		std::string m_Name;
		std::string m_Path;
		std::string m_MetaPath;

		bool m_Active;
		int m_GameObjectID;

		math::OBB m_OBB;
		math::AABB m_AABB;		
	};

}

#endif // !_GAMEOBJECT_H_
