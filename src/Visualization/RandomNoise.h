#pragma once
#include <glm/glm.hpp>
#include "Utils/Resources.h"

class RandomNoise
{
public:
	RandomNoise(int dimensions);
	~RandomNoise();
	glm::vec3 perlin(glm::vec3 position);

private:
	inline void lerp(glm::vec3 a, glm::vec3 b, float w, glm::vec3& r)
	{
		r = a * (1 - w) + b * w;
	}
	glm::vec3*** grid;
	glm::vec3 x0y0;
	glm::vec3 x0y1;
	glm::vec3 x1y0;
	glm::vec3 x1y1;
	glm::vec3 x0;
	glm::vec3 x1;
	glm::vec3 result;
	int dimension;
};