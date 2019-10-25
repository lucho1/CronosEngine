#include "Providers/cnpch.h"
#include "GameObject.h"



namespace Cronos {

	GameObject::GameObject(std::string name, bool start_enabled, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) : m_Name(name), m_Active(start_enabled)
	{
		Component* Comp = CreateComponent(ComponentType::TRANSFORM);
		((ComponentTransform*)(Comp))->SetPosition(position);
		((ComponentTransform*)(Comp))->SetRotation(rotation);
		((ComponentTransform*)(Comp))->SetScale(scale);
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
		if(m_Active == true)
			for (auto& comp : m_Components)
				comp->Update(dt);
	}

	Component* GameObject::CreateComponent(ComponentType type) 
	{
		Component* ret = nullptr;
		switch (type)
		{
			case Cronos::ComponentType::NONE:
				break;
			case Cronos::ComponentType::TRANSFORM:
				ret = new ComponentTransform(this);
				break;
			case Cronos::ComponentType::MESH:
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