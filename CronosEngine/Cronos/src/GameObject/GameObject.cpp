#include "Providers/cnpch.h"
#include "GameObject.h"
#include "Components/ComponentTransform.h"


namespace Cronos {

	GameObject::GameObject(std::string name, bool start_enabled,glm::vec3 position,glm::vec3 rotation,glm::vec3 Scale):m_Name(name),m_Active(start_enabled)
	{
		Component* Comp = CreateComponent(ComponentType::Transform);
		((ComponentTransform*)(Comp))->setPosition(position);
		((ComponentTransform*)(Comp))->setRotation(position);
		((ComponentTransform*)(Comp))->setScale(position);
		m_Components.push_back(Comp);
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

	void GameObject::Update(float dt) 
	{
		for (auto& comp : m_Components) {
			comp->Update(dt);
		}
	}

	Component* GameObject::CreateComponent(ComponentType type) 
	{
		Component* ret = nullptr;
		switch (type)
		{
		case Cronos::ComponentType::none:
			break;
		case Cronos::ComponentType::Transform:
			ret = new ComponentTransform(this);
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
		return ret;
	}
	
}