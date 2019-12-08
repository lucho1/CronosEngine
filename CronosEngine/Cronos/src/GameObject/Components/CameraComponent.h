#ifndef _CAMERACOMPONENT_H_
#define _CAMERACOMPONENT_H_

#include "Component.h"
#include "Helpers/Camera.h"

namespace Cronos {

	class CameraComponent : public Component, public Camera
	{
	public:

		CameraComponent(GameObject* attachedGO);
		~CameraComponent() {}

		virtual void Update(float dt);
		static ComponentType GetType() { return ComponentType::CAMERA; };

	private:

		void Recalculate() override;
	};
}

#endif