#include "Providers/cnpch.h"
#include "GameObject.h"
#include "Components/ComponentTransform.h"


namespace Cronos {

	GameObject::GameObject(std::string name, bool start_enabled):m_Name(name),m_Active(start_enabled)
	{

	}

	GameObject::~GameObject()
	{

	}

	void GameObject::Enable() 
	{
		if (!m_Active) {
			m_Active = true;
			for (auto& comp : m_Components) {
				if(!comp->isEnabled())
					comp->Enable();
			}
		}
	}

	void GameObject::Disable()
	{
		if (m_Active) {
			m_Active = false;
			for (auto& comp : m_Components) {
				if (comp->isEnabled())
					comp->Disable();
			}
		}
	}

	void GameObject::Update() 
	{
		for (auto& comp : m_Components) {
			comp->Update();
		}
	}

	void GameObject::CreateComponent(ComponentType type) 
	{
		switch (type)
		{
		case Cronos::ComponentType::none:
			break;
		case Cronos::ComponentType::Transform:
			Component* ret = new ComponentTransform();
			m_Components.push_back(ret);
			break;
		case Cronos::ComponentType::Mesh:
			break;
		case Cronos::ComponentType::MeshRenderer:
			break;
		case Cronos::ComponentType::Material:
			break;
		default:
			break;
		}

	}

}