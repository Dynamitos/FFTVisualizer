#pragma once
#include "Resources.h"
namespace GL
{
	class Camera
	{
	public:
	private:
		float angleAroundPlayer = 0;
		glm::mat4 viewMatrix;

		glm::vec3 panDirection;
		glm::vec3 position;
		float pitch;
		float yaw;
		float roll;
	};
}