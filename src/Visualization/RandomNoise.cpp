#include "RandomNoise.h"


RandomNoise::RandomNoise(int dimensions)
	: dimension(dimensions)
{
	grid = new glm::vec3**[dimensions];
	for (int x = 0; x < dimensions; ++x)
	{
		grid[x] = new glm::vec3 * [dimensions];
		for (int y = 0; y < dimensions; ++y)
		{
			grid[x][y] = new glm::vec3[dimensions];
			for (int z = 0; z < dimensions; ++z)
			{
				grid[x][y][z] = glm::vec3(random(-1, 1), random(-1, 1), random(-1, 1));
			}
		}
	}
}

RandomNoise::~RandomNoise()
{
	for (int x = 0; x < dimension; ++x)
	{
		for (int y = 0; y < dimension; ++y)
		{
			delete[] grid[x][y];
		}
		delete[] grid[x];
	}
	delete[] grid;
}

glm::vec3 RandomNoise::perlin(glm::vec3 position)
{
	position += dimension / 2;
	if (position.x < 0 || position.y < 0 || position.z < 0 || position.x >= dimension - 1 || position.y >= dimension - 1 || position.z >= dimension - 1)
	{
		return glm::vec3();
	}
	int ix0 = (int)position.x;
	int ix1 = ix0 + 1;
	int iy0 = (int)position.y;
	int iy1 = iy0 + 1;
	int iz0 = (int)position.z;
	int iz1 = iz0 + 1;

	float sx = position.x - (float)ix0;
	float sy = position.y - (float)iy0;
	float sz = position.z - (float)iz0;

	glm::vec3 x0y0z0 = grid[ix0][iy0][iz0];
	glm::vec3 x0y0z1 = grid[ix0][iy0][iz1];
	glm::vec3 x0y1z0 = grid[ix0][iy1][iz0];
	glm::vec3 x0y1z1 = grid[ix0][iy1][iz1];
	glm::vec3 x1y0z0 = grid[ix1][iy0][iz0];
	glm::vec3 x1y0z1 = grid[ix1][iy0][iz1];
	glm::vec3 x1y1z0 = grid[ix1][iy1][iz0];
	glm::vec3 x1y1z1 = grid[ix1][iy1][iz1];
	
	lerp(x0y0z0, x0y0z1, sz, x0y0);
	lerp(x0y1z0, x0y1z1, sz, x0y1);
	lerp(x1y0z0, x1y0z1, sz, x1y0);
	lerp(x1y1z0, x1y1z1, sz, x1y1);

	lerp(x0y0, x0y1, sy, x0);
	lerp(x1y0, x1y1, sy, x1);

	lerp(x0, x1, sx, result);
	return result;
}
