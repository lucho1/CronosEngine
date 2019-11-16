#ifndef _TRANSFORMCOMPONENT_H_
#define _TRANSFORMCOMPONENT_H_

#include "glm/glm.hpp"
#include "Component.h"
#include "MaterialComponent.h"

#include <AABB.hpp>

namespace Cronos {

	class GameObject;

	class TransformComponent : public Component
	{
	public:

		TransformComponent(GameObject* attachedGO, bool active = true);
		~TransformComponent();

		virtual void OnStart() override;
		virtual void Update(float dt) override;

		void SetPosition(glm::vec3 pos) { m_Position = pos; }
		void SetRotation(glm::vec3 rot) { m_Rotation = rot; }
		void SetScale(glm::vec3 scale) { m_Scale = scale; }

		const inline glm::vec3 GetPosition() const { return m_Position; };
		const inline glm::vec3 GetRotation() const { return m_Rotation; };
		const inline glm::vec3 GetScale() const { return m_Scale; };

		//AABBs
		void SetAABB(const glm::vec3& minVec, const glm::vec3& maxVec) { m_ContainerAABBCube = AABB(minVec, maxVec); }
		void SetAABB(const AABB& aabbCube) { m_ContainerAABBCube = AABB(aabbCube); }

		const inline AABB GetAABB() const { return m_ContainerAABBCube; }
		const inline glm::vec3 GetCentralAxis() const { return m_ContainerAABBCube.getCenter(); }
		bool &SetDrawAxis() { return DrawAxis; }
		void DrawCentralAxis();

		//Type
		static ComponentType GetType() { return ComponentType::TRANSFORM; };

		bool DrawAxis = false;

		glm::mat4 m_TransformationMatrix;

	private:

		glm::vec4 m_TransformationVector;
		



		glm::vec3 m_Position;
		glm::vec3 m_Rotation;
		glm::vec3 m_Scale;

		AABB m_ContainerAABBCube;
	};

}

#endif // !_COMPONENTTRANSFORM_H_
