#include "Providers/cnpch.h"
#include "MeshComponent.h"
#include "TransformComponent.h"

#include "Application.h"

namespace Cronos {

	MeshComponent::MeshComponent(GameObject* attachedGO) :
		Component(ComponentType::MESH , attachedGO)
	{
	}

	MeshComponent::~MeshComponent()
	{
		for (uint i = 0; i < m_VertexVector.size(); i++)
			m_VertexVector.erase(m_VertexVector.begin() + i);
		for (uint i = 0; i < m_IndicesVector.size(); i++)
			m_IndicesVector.erase(m_IndicesVector.begin() + i);

		m_IndicesVector.clear();
		m_VertexVector.clear();

		RELEASE(m_MeshVAO);
		RELEASE(m_MeshIBO);
		RELEASE(m_MeshVBO);
	}

	void MeshComponent::SetupMesh(std::vector<CronosVertex>vertices, std::vector<uint>indices)
	{
		CRONOS_ASSERT(vertices.size() != 0, "THE MESH IS NOT WELL CREATED! YOU NEED TO PROPERLY SETUP THE VERTEX VECTOR! \n Try Calling function()");
		CRONOS_ASSERT(indices.size() != 0, "THE MESH IS NOT WELL CREATED! YOU NEED TO PROPERLY SETUP THE INDICES VECTOR! \n Try Calling function()");
		CRONOS_WARN(m_MeshVAO == nullptr, "Warning! Mesh VAO being created is not nullptr!");
		CRONOS_WARN(m_MeshVBO == nullptr, "Warning! Mesh VBO being created is not nullptr!");
		CRONOS_WARN(m_MeshIBO == nullptr, "Warning! Mesh IBO being created is not nullptr!");

		m_VertexVector = vertices;
		m_IndicesVector = indices;
		
		m_MeshVAO = new VertexArray();
		m_MeshVBO = new VertexBuffer(&m_VertexVector[0], m_VertexVector.size() * sizeof(CronosVertex));

		m_MeshVBO->SetLayout({
			{Cronos::VertexDataType::VEC3F, "a_Position"},
			{Cronos::VertexDataType::VEC3F, "a_Normal"},
			{Cronos::VertexDataType::VEC2F, "a_TexCoords"}
			});
		m_MeshVAO->AddVertexBuffer(*m_MeshVBO);

		m_MeshIBO = new IndexBuffer(&m_IndicesVector[0], m_IndicesVector.size());
		m_MeshVAO->AddIndexBuffer(*m_MeshIBO);
	}

	void MeshComponent::Update(float dt)
	{
		Draw(GetParent()->GetComponent<MaterialComponent>(), true);
	}

	void MeshComponent::Draw(MaterialComponent* material, bool bindMaterial)
	{
		if (!isEnabled())
			return;

		App->renderer3D->RenderSubmit(GetParent());
		if (m_DebugDraw)
		{
			DrawVerticesNormals();
			DrawPlanesNormals();
		}
		if (m_DrawAxis)
			DrawCentralAxis();
	}

	void MeshComponent::DrawVerticesNormals()
	{
		//glPushMatrix();

		//glMultMatrixf(glm::value_ptr(GetParent()->GetComponent<TransformComponent>()->GetLocalTranformationMatrix()));
		//glMultMatrixf(glm::value_ptr(GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix()));
		glm::mat4 matrix = GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix() * GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix();

		std::vector<CronosVertex>::iterator item = m_VertexVector.begin();
		for (; item != m_VertexVector.end(); item++)
		{
			glm::vec3 pos = (*item).Position;
			glm::vec3 norm = (*item).Position + (*item).Normal * 0.2f;

			App->renderer3D->DrawLine(pos, norm, glm::vec3(Blue.r, Blue.g, Blue.b), 2.0f);
		}
	}

	void MeshComponent::DrawPlanesNormals()
	{
		glm::mat4 matrix = GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix() * GetParent()->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix();

		for (uint i = 0; i < m_IndicesVector.size() - 2; i += 3)
		{
			glm::vec3 p1 = m_VertexVector[m_IndicesVector[i]].Position;
			glm::vec3 p2 = m_VertexVector[m_IndicesVector[i + 1]].Position;
			glm::vec3 p3 = m_VertexVector[m_IndicesVector[i + 2]].Position;

			glm::vec3 PlaneNormal = glm::cross(p2 - p1, p3 - p1);
			PlaneNormal = glm::normalize(PlaneNormal);
			PlaneNormal *= 0.5f;

			glm::vec3 TriCenter = { 0, 0, 0 };
			TriCenter.x = (p1.x + p2.x + p3.x) / 3;
			TriCenter.y = (p1.y + p2.y + p3.y) / 3;
			TriCenter.z = (p1.z + p2.z + p3.z) / 3;

			App->renderer3D->DrawLine(TriCenter, TriCenter + PlaneNormal, glm::vec3(Green.r, Green.g, Green.b), 2.0f);
		}
	}

	//Central Axis
	void MeshComponent::DrawCentralAxis()
	{
		float linelength = 1.0f;
		glm::vec3 axis = glm::vec3(0.0f);//m_Parent->GetComponent<TransformComponent>()->GetCentralAxis();
		glm::vec3 offset = axis;

		offset.x += linelength;
		App->renderer3D->DrawLine(axis, offset, glm::vec3(1.0f, 0.0f, 0.0f), 2.0f);

		offset = axis;
		offset.y += linelength;
		App->renderer3D->DrawLine(axis, offset, glm::vec3(0.0f, 1.0f, 0.0f), 2.0f);

		offset = axis;
		offset.z += linelength;
		App->renderer3D->DrawLine(axis, offset, glm::vec3(0.0f, 0.0f, 1.0f), 2.0f);
	}
}
