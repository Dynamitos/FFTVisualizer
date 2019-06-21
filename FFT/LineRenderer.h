#pragma once
#include "LineShader.h"

namespace GL
{
	class LineRenderer
	{
	public:
		LineRenderer();
		void init(AudioVisualizerInfo visualizerInfo);
		void render(float* data);
	private:
		GLuint vaoID;
		GLuint displacementVBO;
		GLuint baseVBO;
		LineShader* shader;
		Line line;
		float* dataBuffer;
		uint64_t numPoints;
	};
}