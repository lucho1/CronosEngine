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
		//AABBs---------------------------------------------------------------
		glColor3f(Blue.r, Blue.g, Blue.b);

		App->renderer3D->DrawCube(m_ContainerAABBCube.getMin(), m_ContainerAABBCube.getMax(),
			glm::vec3(Blue.r, Blue.g, Blue.b), 2.0f, m_LocalTransformationMatrix);

		//glMultMatrixf(glm::value_ptr(m_GlobalTransformationMatrix));
		//glMultMatrixf(glm::value_ptr(m_Parent->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix() * m_LocalTransformationMatrix));

		//OBB m_ContainerOOBB = OBB(m_ContainerAABBCube);

		//glMultMatrixf(glm::value_ptr(m_GlobalTransformationMatrix));
		//glPushMatrix();
		//glMultMatrixf(glm::value_ptr(m_Parent->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix() * m_LocalTransformationMatrix));

		glm::vec3 corners[8];
		m_ContainerOOBB.getCorners(corners);
		App->renderer3D->DrawCube(corners[1], corners[4],
			glm::vec3(Green.r, Green.g, Green.b), 2.0f, m_LocalTransformationMatrix);

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
		AABBPos = m_Translation;
		m_Translation = position;
		UpdateTransform();
	}

	void TransformComponent::SetScale(glm::vec3 scale)
	{
		AABBScale = m_Scale;
		m_Scale = scale;
		UpdateTransform();
	}

	//Set the orientation of the object (pass Euler Angles in degrees!!)
	void TransformComponent::SetOrientation(glm::vec3 euler_angles)
	{
		AABBOrientation = m_Orientation;
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

		m_ContainerOOBB = OBB(m_ContainerAABBCube);
		
		//AABB Transform ------------------------------------------------------------------------------------
		//glm::mat4 resTrans = glm::translate(glm::mat4(1.0f), (m_Translation - AABBPos));
		//AABBPos = m_Translation;
		//
		//if (glm::length(m_Scale - AABBScale) > 0.1f)
		//{
		//	//glm::mat4 matscale = glm::scale(glm::mat4(1.0f), (AABBScale + (m_Scale - AABBScale)));
		//	//glm::mat4 matscale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) + m_Scale - AABBScale);
		//	glm::mat4 matscale = glm::scale(glm::mat4(1.0f), (glm::vec3(1.0f) + (m_Scale - AABBScale)));
		//	resTrans = resTrans * matscale;
		//	AABBScale = m_Scale;
		//}
		//
		//m_ContainerAABBCube.transform(resTrans);
		//AABB Transform ------------------------------------------------------------------------------------
		

		//That code up ^ it works :) (more or less)
		//AABB Transform ------------------------------------------------------------------------------------

		glm::mat4 matScale = glm::mat4(1.0f);
		glm::quat qRot = glm::quat(1.0f, glm::vec3(0.0f));

		glm::mat4 resTrans = glm::translate(glm::mat4(1.0f), (m_Translation - AABBPos));
		AABBPos = m_Translation;

		if (glm::length(m_Scale - AABBScale) > 0.1f)
		{
			//glm::mat4 matscale = glm::scale(glm::mat4(1.0f), (AABBScale + (m_Scale - AABBScale)));
			//glm::mat4 matscale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f) + m_Scale - AABBScale);
			matScale = glm::scale(glm::mat4(1.0f), (glm::vec3(1.0f) + (m_Scale - AABBScale)));
			//resTrans = resTrans * matScale;
			AABBScale = m_Scale;
		}

		//glm::vec3 EA_Rad = glm::radians(euler_angles);
		//glm::quat rot = glm::quat(EA_Rad - m_EulerAngles);
		//
		//m_Orientation = m_Orientation * rot;
		//m_EulerAngles = EA_Rad;
		
		
		//abs(q1.dot(q2)) > 1-EPS
		if (glm::abs(glm::dot(AABBOrientation, m_Orientation)) < (1.0f - 0.002f))
		{
			qRot = glm::quat(m_EulerAngles - glm::eulerAngles(AABBOrientation));
			AABBOrientation = AABBOrientation * qRot;
			//resTrans = resTrans * glm::mat4_cast(AABBOrientation) * matscale;
		}
		
		resTrans = resTrans * glm::mat4_cast(qRot) * matScale;
		m_ContainerAABBCube.transform(resTrans);

		//AABB Transform ------------------------------------------------------------------------------------

		bool changed = false;

		//Update childs' transform
		GameObject* GOAttached_Parent = GetParent()->GetParentGameObject();
		if (GOAttached_Parent != nullptr)
			m_GlobalTransformationMatrix = GOAttached_Parent->GetComponent<TransformComponent>()->GetGlobalTranformationMatrix() * m_LocalTransformationMatrix;
		else
			m_GlobalTransformationMatrix = m_LocalTransformationMatrix;

		for (auto child : GetParent()->m_Childs)
			child->GetComponent<TransformComponent>()->UpdateTransform();



		
		
		
		//glm::scale(glm::mat4(1.0f), (m_Scale - AABBScale))
			
		//}

		//if((m_Translation - prevAABB_Pos).length() > 0.2f)
		//	m_ContainerAABBCube.transform(glm::translate(glm::mat4(1.0f), m_Translation));

		//m_ContainerAABBCube.updateMinMax()

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
}