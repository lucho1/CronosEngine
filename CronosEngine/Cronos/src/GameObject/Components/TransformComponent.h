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

		//Type
		static ComponentType GetType() { return ComponentType::TRANSFORM; };

		//AABBs & Axis
		void SetAABB(const glm::vec3& minVec, const glm::vec3& maxVec) { m_ContainerAABBCube = AABB(minVec, maxVec); }
		void SetAABB(const AABB& aabbCube) { m_ContainerAABBCube = AABB(aabbCube); }
		
		bool &SetDrawAxis() { return DrawAxis; }
		void DrawCentralAxis();

		const inline AABB GetAABB() const { return m_ContainerAABBCube; }
		const inline glm::vec3 GetCentralAxis() const { return m_ContainerAABBCube.getCenter(); }

	public:

		//This is to SET the game object to a desired transformation
		void SetPosition(glm::vec3 pos);
		void SetScale(glm::vec3 scale);
		void SetOrientation(glm::vec3 eulerAxAngles);
		
		//This is to ADD the game object a transformation
		void Move(glm::vec3 translation);
		void Scale(glm::vec3 scale);
		void Rotate(glm::vec3 eulerAxAngles);

		//Getters
		const inline glm::vec3 GetPosition() const { return m_Position; };
		const inline glm::vec3 GetScale() const { return m_Scale; };
		const inline glm::vec3 GetOrientation() const { return m_Orientation_eulerAngles; };
		//const inline glm::vec3 GetRotationVector() const { return glm::quat::value_type(m_Rotation); }

	private:

		void DecomposeTransformation();

	public:

		bool DrawAxis = false;
		glm::mat4 m_TransformationMatrix;

	private:

		glm::vec3 m_Position;
		glm::vec3 m_Scale;
		glm::quat m_Orientation;
		glm::vec3 m_Orientation_eulerAngles;

		AABB m_ContainerAABBCube;
	};

}

#endif // !_COMPONENTTRANSFORM_H_
