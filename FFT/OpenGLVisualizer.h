#pragma once
#include "Resources.h"
#include "AudioVisualizer.h"

struct RawModel
{
	RawModel(GLuint vaoID, GLsizei vertexCount, const char* name)
		: vaoID(vaoID)
		, vertexCount(vertexCount)
		, name(name)
	{}
	GLuint vaoID;
	GLsizei vertexCount;
	const char* name;
};


class OpenGLRenderer : public AudioVisualizer
{
public:
	OpenGLRenderer();
	virtual ~OpenGLRenderer();

	virtual void init();
	virtual void renderData(std::unique_ptr<SampleContainer> data);
};