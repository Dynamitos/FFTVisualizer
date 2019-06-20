#include "Loader.h"
#include "Visualizer.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

bool Input::keys[GLFW_KEY_LAST];

using namespace GL;

RawModel* Loader::loadToVAO(float* vertices, uint64_t vertexSize, float* texCoords, uint64_t texCoordSize)
{
	GLuint vao = createVAO();
	storeDataInAttributeList(0, 3, vertices, vertexSize);
	storeDataInAttributeList(1, 2, texCoords, texCoordSize);
	unbindVAO();
	GLuint numVertices = vertexSize / sizeof(float);
	return new RawModel(vao, numVertices / 3, nullptr);
}

RawModel* Loader::loadToVAO(float* vertices, uint64_t vertexSize, float* texCoords, uint64_t texCoordSize, float* normals, uint64_t normalSize, uint16_t* indices, uint64_t indexSize, const char* name)
{
	GLuint vao = createVAO();
	bindIndicesBuffer(indices, indexSize);
	storeDataInAttributeList(0, 3, vertices, vertexSize);
	storeDataInAttributeList(1, 2, texCoords, texCoordSize);
	storeDataInAttributeList(2, 3, normals, normalSize);
	unbindVAO();
	GLuint numIndices = indexSize / sizeof(uint16_t);
	return new RawModel(vao, numIndices, nullptr);
}

RawModel* Loader::loadToVAO(float* vertices, uint64_t vertexSize, int dimensions)
{
	GLuint vao = createVAO();
	storeDataInAttributeList(0, dimensions, vertices, vertexSize);
	unbindVAO();
	GLuint numVertices = vertexSize / sizeof(float);
	return new RawModel(vao, numVertices / dimensions, nullptr);
}

GLuint Loader::loadTexture(const char* fileName)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	int x, y, channels = 4;
	std::string textureBasePath = "./_Game/textures/";
	GLubyte* data = stbi_load(textureBasePath.append(fileName).c_str(), &x, &y, &channels, 4);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
	textures.push_back(textureID);
	return textureID;
}

GLuint Loader::loadTexture(uint32_t* pixels, size_t pixelSize, uint16_t width)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, width, 0, GL_RGBA, GL_UNSIGNED_INT, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, -0.4f);
	textures.push_back(textureID);
	return textureID;
}

GLuint Loader::createVAO()
{
	GLuint vaoID;
	glGenVertexArrays(1, &vaoID);
	vaos.push_back(vaoID);
	glBindVertexArray(vaoID);
	return vaoID;
}

void Loader::cleanUp()
{
	glDeleteVertexArrays(vaos.size(), vaos.data());
	glDeleteBuffers(vbos.size(), vbos.data());
	glDeleteTextures(textures.size(), textures.data());
}

void Loader::storeDataInAttributeList(GLint attributeNumber, int coordinateSize, float* data, uint64_t dataSize)
{
	GLuint vboID;
	glGenBuffers(1, &vboID);
	vbos.push_back(vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
	glVertexAttribPointer(attributeNumber, coordinateSize, GL_FLOAT, false, 0, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Loader::unbindVAO()
{
	glBindVertexArray(0);
}

void Loader::bindIndicesBuffer(uint16_t* indices, uint64_t indexSize)
{
	GLuint vboID;
	glGenBuffers(1, &vboID);
	vbos.push_back(vboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indices, GL_STATIC_DRAW);
}



