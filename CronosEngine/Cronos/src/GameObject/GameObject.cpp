#include "Providers/cnpch.h"
#include "GameObject.h"
#include "Application.h"
#include "Modules/Filesystem.h"
#include "Components/TransformComponent.h"
#include "Components/MaterialComponent.h"
#include "Components/CameraComponent.h"

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

			App->renderer3D->DrawCube(max, min, glm::vec3(0.0f, 0.0, 1.0f), 1.2f/*, GetComponent<TransformComponent>()->GetGlobalTranformationMatrix()*/);
			
			//OOBB Draw
			math::float3 corners[8] = { float3(0, 0, 0) };
			m_OBB.GetCornerPoints(corners);
			max = glm::vec3(corners[7].x, corners[7].y, corners[7].z);
			min = glm::vec3(corners[0].x, corners[0].y, corners[0].z);

			App->renderer3D->DrawCube(max, min, glm::vec3(0.0f, 1.0f, 0.0f), 2.0f/*, GetComponent<TransformComponent>()->GetGlobalTranformationMatrix()*/);						
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

	//---------------------------------------------
	void GameObject::SetOOBBTransform(glm::vec3 translation, glm::quat orientation, glm::vec3 scale)
	{
		/*if (GetComponent<TransformComponent>() == nullptr)
			return;

		glm::vec3 boxPos = glm::vec3(m_OBB.pos.x, m_OBB.pos.y, m_OBB.pos.z);
		glm::vec3 GObjPos = GetComponent<TransformComponent>()->GetTranslation();

		m_AABBTranslation = transform - ((boxPos + GObjPos) - boxPos);
		this;
		
		if (glm::length(m_AABBTranslation) <= 0.2f)
			return;

		//glm::mat4 m = transform;
		glm::mat4 m = glm::translate(glm::mat4(1.0f), m_AABBTranslation) /**
			glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), transform2);*/

		
		glm::mat4 m = glm::translate(glm::mat4(1.0f), translation) *
			glm::mat4(1.0f) * glm::scale(glm::mat4(1.0f), scale);
		
		glm::mat4 resMat = glm::transpose(m);

		//glm::vec4 zRow = resMat[2];
		//resMat[2] = resMat[1];
		//resMat[1] = zRow;

		//resMat[2][1] = -resMat[2][1];

		/*glm::vec2 XYAux = glm::vec2(resMat[0][3], resMat[2][3]);

		glm::vec4 xRow = resMat[0];
		resMat[0] = resMat[2];
		resMat[2] = xRow;

		resMat[0][3] = XYAux.x;
		resMat[2][3] = XYAux.y;*/


		math::float4x4 mat = math::float4x4::identity;
		mat.Set(glm::value_ptr(resMat));

		m_OBB = m_AABB;
		m_OBB.Transform(mat);

		m_AABB.SetNegativeInfinity();
		m_AABB.Enclose(m_OBB);
	}
}