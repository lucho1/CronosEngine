#include"Providers/cnpch.h"
#include "TransformComponent.h"
#include "GameObject/GameObject.h"

#include "Application.h"



namespace Cronos {

	TransformComponent::TransformComponent(GameObject* attachedGO, bool active)
		: Component(ComponentType::TRANSFORM, attachedGO, active)
	{
		m_EulerAngles = glm::vec3(0.0f);
		m_Orientation = glm::quat(1.0f, glm::vec3(0.0f));
		m_Scale = m_Translation = glm::vec3(0.0f);

		m_GlobalTransformationMatrix = m_LocalTransformationMatrix = glm::mat4(1.0f);
		UpdateTransform();

		/*if(!m_ContainerAABBCube.isNull())
			TrueAABB = new BoundingBox(attachedGO);*/
	}

	TransformComponent::~TransformComponent()
	{
	}

	void TransformComponent::Update(float dt)
	{
		App->scene->BasicTestShader->Bind();
		App->scene->BasicTestShader->SetUniformMat4f("u_Model", m_GlobalTransformationMatrix);
		App->scene->BasicTestShader->Unbind();


		//AABBs---------------------------------------------------------------
		//glm::vec3 aabbTrans = m_ContainerAABBCube.getCenter();
		//m_ContainerAABBCube.translate(m_Translation - aabbTrans);
		//m_ContainerAABBCube.scale(glm::vec3(0.0f), m_Scale);

		glColor3f(Blue.r, Blue.g, Blue.b);

		glPushMatrix();

		glMultMatrixf(glm::value_ptr(m_LocalTransformationMatrix));
		glMultMatrixf(glm::value_ptr(m_GlobalTransformationMatrix));

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetProjectionMatrix()));
		//glPushMatrix();
		//glMultMatrixf(glm::value_ptr(m_Parent->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix() * m_LocalTransformationMatrix));
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetViewMatrix()));
		App->renderer3D->DrawCube(m_ContainerAABBCube.getMin(), m_ContainerAABBCube.getMax());

		


		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		/*glPushMatrix();
		glMultMatrixf(glm::value_ptr(m_GlobalTransformationMatrix));
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetProjectionMatrix()));
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetViewMatrix()));
		glm::vec3 min = m_ContainerAABBCube.getMax();
		glm::vec3 max = m_ContainerAABBCube.getMax();

		glm::vec3 v1 = max - glm::vec3(0.0f, 0.0f, min.z);
		App->renderer3D->DrawQuad(min, v1);
		glm::vec3 v2 = max - glm::vec3(0.0f, max.y - min.y, 0.0f);
		App->renderer3D->DrawQuad(v1, v2);
		glm::vec3 v3 = max - glm::vec3(max.x - min.x, 0.0f, 0.0f);
		App->renderer3D->DrawQuad(v2, v3);
		App->renderer3D->DrawQuad(v3, min);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPopMatrix();*/
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		/*if(!m_ContainerAABBCube.isNull() && TrueAABB != nullptr && !TrueAABB->IsNull())
			TrueAABB->Update(dt);

		if (!m_ContainerAABBCube.isNull() && TrueAABB == nullptr)
			TrueAABB = new BoundingBox(m_Parent);*/

		//Just testing if AABB is working
		/*glPushMatrix();
		glMultMatrixf(glm::value_ptr(m_GlobalTransformationMatrix));
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetProjectionMatrix()));
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetViewMatrix()));

		App->renderer3D->DrawQuad(GetAABB().getMin(), GetAABB().getMax());

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glPopMatrix();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);*/
	}

	void TransformComponent::SetPosition(glm::vec3 position)
	{
		m_ContainerAABBCube.translate(position - m_Translation);
		m_Translation = position;
		UpdateTransform();
	}

	void TransformComponent::SetScale(glm::vec3 scale)
	{

		if(scale.x >= 1.2f && scale.y >= 1.2f && scale.z >= 1.2f)
			m_ContainerAABBCube.scale(scale - m_Scale, m_ContainerAABBCube.getCenter());

		m_Scale = scale;
		UpdateTransform();
	}

	//Set the orientation of the object (pass Euler Angles in degrees!!)
	void TransformComponent::SetOrientation(glm::vec3 euler_angles)
	{
		glm::vec3 EA_Rad = glm::radians(euler_angles);
		glm::quat rot = glm::quat(EA_Rad - m_EulerAngles);

		m_Orientation = m_Orientation * rot;
		m_EulerAngles = EA_Rad;
		UpdateTransform();
	}

	void TransformComponent::Rotate(glm::vec3 euler_angles)
	{
		glm::vec3 EA_Rad = glm::radians(euler_angles);
		glm::quat newOrientation = glm::quat(EA_Rad);

		m_Orientation = m_Orientation * newOrientation;
		m_EulerAngles = EA_Rad;
	}

	void TransformComponent::Move(glm::vec3 translation)
	{
		m_Translation += translation;
		UpdateTransform();
	}

	void TransformComponent::Scale(glm::vec3 scale)
	{
		m_Scale += scale;
		UpdateTransform();
	}

	void TransformComponent::UpdateTransform()
	{
		m_LocalTransformationMatrix = glm::translate(glm::mat4(1.0f), m_Translation) *
			glm::mat4_cast(m_Orientation) * glm::scale(glm::mat4(1.0f), m_Scale);

		//Update childs' transform
		GameObject* GOAttached_Parent = GetParent()->GetParentGameObject();
		if (GOAttached_Parent != nullptr)
			m_GlobalTransformationMatrix = GOAttached_Parent->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix() * m_LocalTransformationMatrix;
		else
			m_GlobalTransformationMatrix = m_LocalTransformationMatrix;

		for (auto child : GetParent()->m_Childs)
			child->GetComponent<TransformComponent>()->UpdateTransform();




		//glm::vec3 v = glm::vec3(0.0f);
		//glm::vec3 trans = glm::vec3(0.0f);
		//glm::decompose(m_GlobalTransformationMatrix, v, glm::quat(1.0f, v), trans, v, glm::vec4(0.0f));
		//
		//glm::vec3 res = trans - m_Translation;
		//if(res.length() > 0.0f)
			//m_ContainerAABBCube.translate(m_Translation + m_ContainerAABBCube.getCenter());

		//
		//m_ContainerAABBCube.translate(m_Translation - (m_ContainerAABBCube.getDiagonal()));

		//static glm::vec3 tttt = m_ContainerAABBCube.getCenter() /*+ glm::vec3(0.0f, m_ContainerAABBCube.getMax().y, 0.0f)*/;
		//trans = glm::vec3(trans.x, m_ContainerAABBCube.getMax().y, trans.z);
		//m_ContainerAABBCube.translate(trans - tttt);
	}

	//Central Axis
	void TransformComponent::DrawCentralAxis()
	{
		float linelength = 1.0f;
		glm::vec3 axis = GetCentralAxis();

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetProjectionMatrix()));

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(glm::value_ptr(App->engineCamera->GetViewMatrix()));

		glLineWidth(5.0f);
		glBegin(GL_LINES);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glVertex3f(axis.x, axis.y, axis.z);
		glVertex3f(axis.x + linelength, axis.y, axis.z);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glVertex3f(axis.x, axis.y, axis.z);
		glVertex3f(axis.x, axis.y + linelength, axis.z);
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glVertex3f(axis.x, axis.y, axis.z);
		glVertex3f(axis.x, axis.y, axis.z + linelength);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glEnd();
		glLineWidth(2.0f);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	//------------------------------------------------------------------------------------------
	//BoundingBox::BoundingBox(GameObject* GameObjectAttached) : GOAttached(GameObjectAttached)
	//{
	//	AABB GOAABB = GOAttached->GetComponent<TransformComponent>()->GetAABB();
	//	glm::vec3 size = glm::abs(GOAABB.getMax() - GOAABB.getMin());

	//	//PrimitiveGameObject* ret = new PrimitiveGameObject(PrimitiveType::CUBE, "Cube", { 1,1,1 });
	//	OBBox = new PrimitiveGameObject(PrimitiveType::CUBE, GOAttached->GetName() + " OBB", size,
	//		GOAttached->GetComponent<TransformComponent>()->GetTranslation());

	//	AABBox = new PrimitiveGameObject(PrimitiveType::CUBE, GOAttached->GetName() + " AABB", size + glm::vec3(0.3f),
	//		GOAttached->GetComponent<TransformComponent>()->GetTranslation());
	//}

	//void BoundingBox::Update(float dt)
	//{
	//	if (OBBox == nullptr || AABBox == nullptr)
	//		return;

	//	TransformComponent* OBBComp = OBBox->GetComponent<TransformComponent>();
	//	TransformComponent* AABBComp = AABBox->GetComponent<TransformComponent>();
	//	TransformComponent* GOComp = GOAttached->GetComponent<TransformComponent>();

	//	OBBComp->SetOrientation(GOComp->GetOrientation());
	//	OBBComp->SetScale(GOComp->GetScale());
	//	OBBComp->SetPosition(GOComp->GetTranslation());

	//	AABBComp->SetPosition(OBBComp->GetTranslation());
	//	AABBComp->SetScale(OBBComp->GetScale() + glm::vec3(0.3f));

	//	glLineWidth(3.5f);
	//	glColor3f(Blue.r, Blue.g, Blue.b);
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//	OBBox->Update(dt);
	//	glColor3f(Green.r, Green.g, Green.b);
	//	AABBox->Update(dt);
	//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//	glLineWidth(2.0f);
	//}

}