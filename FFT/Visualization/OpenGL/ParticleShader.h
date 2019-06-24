#pragma once
#include "ShaderProgram.h"

namespace GL
{
	class ParticleShader : public ShaderProgram
	{
	public:
		ParticleShader();

		void loadViewMatrix(glm::mat4 viewMatrix);
		void loadProjectionMatrix(glm::mat4 projectionMatrix);
		void loadTexture();
		// Inherited via ShaderProgram
		virtual void getAllUniformLocations() override;
		virtual void bindAttributes() override;
	private:
		const char* VERT_FILE = "particleVertex.shader";
		const char* FRAG_FILE = "particleFragment.shader";
		GLuint location_texture;
		GLuint location_projectionMatrix;
		GLuint location_viewMatrix;
	};
}