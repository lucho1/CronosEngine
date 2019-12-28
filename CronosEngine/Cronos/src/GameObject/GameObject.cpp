#include "Providers/cnpch.h"
#include "GameObject.h"
#include "Application.h"

#include "Modules/Filesystem.h"
#include "Renderer/GLRenderer3D.h"
#include "ImGui/ImGuiLayer.h"

#include "Components/TransformComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/CameraComponent.h"
#include "Components/LightComponent.h"

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

			//AABB Draw
			glm::vec3 max = glm::vec3(m_AABB.maxPoint.x, m_AABB.maxPoint.y, m_AABB.maxPoint.z);
			glm::vec3 min = glm::vec3(m_AABB.minPoint.x, m_AABB.minPoint.y, m_AABB.minPoint.z);
			glm::vec3 color = glm::vec3(0.93f, 0.93f, 0.93f);

			if (App->EditorGUI->GetCurrentGameObject() == this)
				color = glm::vec3(0.87f, 0.83f, 0.04f);

			App->renderer3D->DrawCube(max, min, color, 1.2f/*, GetComponent<TransformComponent>()->GetGlobalTranformationMatrix()*/);

			////OOBB Draw
			//math::float3 corners[8] = { float3(0, 0, 0) };
			//m_OOBB.GetCornerPoints(corners);
			//max = glm::vec3(corners[7].x, corners[7].y, corners[7].z);
			//min = glm::vec3(corners[0].x, corners[0].y, corners[0].z);
			
			//App->renderer3D->DrawCube(max, min, glm::vec3(0.0f, 1.0f, 0.0f), 2.0f, GetComponent<TransformComponent>()->GetLocalTranformationMatrix());
			//App->renderer3D->DrawRotatedCube(max, min, GetComponent<TransformComponent>()->GetOrientationQuaternion(), glm::vec3(0.0f, 1.0f, 0.0f), 2.0f/*, GetComponent<TransformComponent>()->GetGlobalTranformationMatrix()*/);
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
			case Cronos::ComponentType::CAMERA:
				ret = new CameraComponent(this);
				break;
			case Cronos::ComponentType::LIGHT:
				ret = new LightComponent(this);
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

	void GameObject::SetParent(GameObject* Go)
	{
		TransformComponent* comp = GetComponent<TransformComponent>();
		glm::vec3 pos, scale;
		glm::quat rot;

		if (Go == nullptr)	
			glm::decompose(comp->GetGlobalTranformationMatrix(), scale, rot, pos, glm::vec3(), glm::vec4());
		else
		{
			glm::mat4 parentMat = Go->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix();
			glm::mat4 inverseParentMat = glm::inverse(parentMat);
			glm::mat4 localDesiredMat = inverseParentMat * comp->GetGlobalTranformationMatrix();
			
			glm::decompose(localDesiredMat, scale, rot, pos, glm::vec3(), glm::vec4());
		}

		Parent = Go;
		comp->SetPosition(pos);
		comp->SetScale(scale);
		comp->SetOrientation(glm::degrees(glm::eulerAngles(rot)));
	}

	//---------------------------------------------
	void GameObject::SetOOBBTransform(glm::mat4 transform)
	{
		glm::mat4 resMat = glm::transpose(transform);

		math::float4x4 mat = math::float4x4::identity;
		mat.Set(glm::value_ptr(resMat));

		m_OOBB.SetFrom(m_InitialAABB);
		m_OOBB.Transform(mat);
		m_AABB.SetFrom(m_OOBB);
	}
}
