#pragma once
#include "Utils/Resources.h"
namespace GL
{
	class Loader
	{
	public:
		RawModel* loadToVAO(float* vertices, uint64_t vertexSize, float* texCoords, uint64_t texCoordSize);
		RawModel* loadToVAO(float* vertices, uint64_t vertexSize, float* texCoords, uint64_t texCoordSize,
			float* normals, uint64_t normalSize, uint16_t* indices, uint64_t indexSize, const char* name);
		RawModel* loadToVAO(float* vertices, uint64_t vertexSize, int dimensions);
		GLuint loadTexture(const char* fileName);
		GLuint loadTexture(uint32_t* pixels, size_t pixelSize, uint16_t width);
		GLuint createVAO();

		void cleanUp();
		void storeDataInAttributeList(GLint attributeNumber, int coordinateSize, float* data, uint64_t dataSize);
		void unbindVAO();

		void bindIndicesBuffer(uint16_t* indices, uint64_t indexSize);
	private:
		std::vector<GLuint> vaos;
		std::vector<GLuint> vbos;
		std::vector<GLuint> textures;
	};
}