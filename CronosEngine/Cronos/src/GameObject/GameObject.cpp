#include "Providers/cnpch.h"
#include "GameObject.h"
#include "Application.h"
#include "Modules/Filesystem.h"
#include "Components/TransformComponent.h"
#include "Components/MaterialComponent.h"

namespace Cronos {

	GameObject::GameObject(const std::string& name, int gameObjectID, const std::string& path, bool start_enabled, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
		: m_Name(name), m_GameObjectID(gameObjectID), m_Active(start_enabled), m_Path(path)
	{
		Component* Comp = CreateComponent(ComponentType::TRANSFORM);
		((TransformComponent*)(Comp))->SetPosition(position);
		((TransformComponent*)(Comp))->SetOrientation(rotation);
		((TransformComponent*)(Comp))->SetScale(scale);
		m_Components.push_back(Comp);
		m_MetaPath = App->filesystem->GetMetaPath() + std::to_string(gameObjectID) + ".model";
	}

	GameObject::~GameObject()
	{
	}

	//---------------------------------------------
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

	void GameObject::Update(float dt)
	{
		if (m_Active == true)
		{
			for (auto& comp : m_Components)
				comp->Update(dt);

			for (auto& child : m_Childs)
				child->Update(dt);

			glm::vec3 max = glm::vec3(m_AABB.maxPoint.x, m_AABB.maxPoint.y, m_AABB.maxPoint.z);
			glm::vec3 min = glm::vec3(m_AABB.minPoint.x, m_AABB.minPoint.y, m_AABB.minPoint.z);

			App->renderer3D->DrawCube(max, min);
		}
	}

	//---------------------------------------------
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

	//---------------------------------------------
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
				ret = new MaterialComponent(this);
				break;
			default:
				break;
		}
		return ret;
	}

	void GameObject::SetNewID()
	{
		m_GameObjectID = App->m_RandomNumGenerator.GetIntRN();
		m_MetaPath = App->filesystem->GetMetaPath() + std::to_string(m_GameObjectID) + ".model";

		for (auto& childs : m_Childs) 
			childs->SetNewID();		
	}
}