#ifndef _OLDER_GL_LIGHT_H_
#define _OLDER_GL_LIGHT_H_

#include "Color.h"

namespace Cronos {

	struct GL_Light
	{
		GL_Light();

		void Init();
		void SetPos(float x, float y, float z);
		void Active(bool active);
		void Render();

		Color ambient;
		Color diffuse;
		glm::vec3 position;

		int ref;
		bool on;
	};

}

#endif