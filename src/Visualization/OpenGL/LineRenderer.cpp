#include "LineRenderer.h"

GL::LineRenderer::LineRenderer()
{
}

void GL::LineRenderer::init(AudioVisualizerInfo visualizerInfo)
{
	line = visualizerInfo.line;
	numPoints = visualizerInfo.numBands * visualizerInfo.tesselationLevel;
	shader = new LineShader();
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
	glGenBuffers(1, &baseVBO);
	float* baseVertices = new float[numPoints * 3];
	float dx = (line.end.x - line.start.x) / numPoints;
	float dy = (line.end.y - line.start.y) / numPoints;
	for (int i = 0; i < numPoints; ++i)
	{
		baseVertices[i * 3 + 0] = line.start.x + i * dx;
		baseVertices[i * 3 + 1] = line.start.y + i * dy;
		baseVertices[i * 3 + 2] = -1;
	}
	glBindBuffer(GL_ARRAY_BUFFER, baseVBO);
	glBufferData(GL_ARRAY_BUFFER, numPoints * 3 * sizeof(float), baseVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	dataBuffer = new float[numPoints];
	glGenBuffers(1, &displacementVBO);
	glBindBuffer(GL_ARRAY_BUFFER, displacementVBO);
	glBufferData(GL_ARRAY_BUFFER, numPoints * sizeof(float), nullptr, GL_STREAM_DRAW);
	glVertexAttribPointer(1, 1, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	shader->start();
	shader->loadColor(line.lineColor);
	shader->stop();
}

void GL::LineRenderer::render(float* data)
{
	shader->start();
	GL_CHECK();
	glBindVertexArray(vaoID);
	GL_CHECK();
	glBindBuffer(GL_ARRAY_BUFFER, displacementVBO);
	GL_CHECK();
	glBufferSubData(GL_ARRAY_BUFFER, 0, numPoints * sizeof(float), data);
	GL_CHECK();
	glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)numPoints);
	GL_CHECK();
	shader->stop();
}
