#include "Providers/cnpch.h"

#include "Providers/Globals.h"
#include "GL_Light.h"

#include <gl/GL.h>
#include "mmgr/mmgr.h"

namespace Cronos {

	GL_Light::GL_Light() : ref(-1), on(false), position(0.0f, 0.0f, 0.0f)
	{}

	void GL_Light::Init()
	{
		glLightfv(ref, GL_AMBIENT, &ambient);
		glLightfv(ref, GL_DIFFUSE, &diffuse);
	}

	void GL_Light::SetPos(float x, float y, float z)
	{
		position.x = x;
		position.y = y;
		position.z = z;
	}

	void GL_Light::Render()
	{
		if (on)
		{
			float pos[] = { position.x, position.y, position.z, 1.0f };
			glLightfv(ref, GL_POSITION, pos);
		}
	}

	void GL_Light::Active(bool active)
	{
		if (on != active)
		{
			on = !on;

			if (on)
				glEnable(ref);
			else
				glDisable(ref);
		}
	}

}