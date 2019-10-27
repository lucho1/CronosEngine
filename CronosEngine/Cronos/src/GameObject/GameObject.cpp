#include "Providers/cnpch.h"
#include "GameObject.h"

#include "Components/TransformComponent.h"
#include "Components/MeshComponent.h"

namespace Cronos {

	GameObject::GameObject(const std::string& name, int gameObjectID, const std::string& path, bool start_enabled, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
		: m_Name(name), m_GameObjectID(gameObjectID), m_Active(start_enabled), m_Path(path)
	{
		Component* Comp = CreateComponent(ComponentType::TRANSFORM);
		((TransformComponent*)(Comp))->SetPosition(position);
		((TransformComponent*)(Comp))->SetRotation(rotation);
		((TransformComponent*)(Comp))->SetScale(scale);
		m_Components.push_back(Comp);
	}

	GameObject::~GameObject()
	{
	}

	void GameObject::CleanUp()
	{
		for (uint i = 0; i < m_Components.size(); i++)
		{
			RELEASE(m_Components[i]);
			m_Components.erase(m_Components.begin() + i);
		}
		for (auto&& element : m_Childs)
			RELEASE(element);

		m_Components.clear();
		m_Childs.clear();
	}

	void GameObject::Enable() 
	{
		if (!m_Active)
		{
			m_Active = true;

			for (auto& comp : m_Components)
				if(!comp->isEnabled())
					comp->Enable();
		}
	}

	void GameObject::Disable()
	{
		if (m_Active)
		{
			m_Active = false;

			for (auto& comp : m_Components)
				if (comp->isEnabled())
					comp->Disable();
		}
	}

	void GameObject::Update(float dt)
	{
		if (m_Active == true)
		{
			for (auto& comp : m_Components)
				comp->Update(dt);

			for (auto& child : m_Childs)
				child->Update(dt);
		}
	}

	void GameObject::SetAABB(const glm::vec3& minVec, const glm::vec3& maxVec)
	{
		auto comp = GetComponent<TransformComponent>();
		if (comp != nullptr)
			comp->SetAABB(minVec, minVec);
	}

	Component* GameObject::CreateComponent(ComponentType type) 
	{
		Component* ret = nullptr;
		switch (type)
		{
			case Cronos::ComponentType::NONE:
				break;
			case Cronos::ComponentType::TRANSFORM:
				ret = new TransformComponent(this);
				break;
			case Cronos::ComponentType::MESH:
				ret = new MeshComponent(this);
				break;
			case Cronos::ComponentType::MESH_RENDERER:
				break;
			case Cronos::ComponentType::MATERIAL:
				break;
			default:
				break;
		}
		return ret;
	}
}