#pragma once
#include "ShaderProgram.h"

namespace GL
{
	class ImageShader : public ShaderProgram
	{
	public:
		ImageShader();
		void loadIntensityMultipliers(float intensityScale, float intensityOffset);
		void loadBass(float bass);
		void loadTransformationMatrix(glm::mat4 transformationMatrix);
		void loadViewMatrix(glm::mat4 viewMatrix);
		void loadTextures();
		void loadLight(glm::vec3 lightPosition, glm::vec4 lightColor);
		void loadProjection(glm::mat4 projection);
	private:
		// Inherited via ShaderProgram
		virtual void getAllUniformLocations() override;
		virtual void bindAttributes() override;

		const char* VERT_FILE = "imageVertex.shader";
		const char* FRAG_FILE = "imageFragment.shader";
		int location_texture;
		int location_transformationMatrix;
		int location_lightPosition;
		int location_lightColor;
		int location_lineColor;
		int location_intensityScale;
		int location_intensityOffset;
		int location_bass;
		int location_lineWidth;
		int location_array;
		int location_projection;
		int location_view;
	};
}