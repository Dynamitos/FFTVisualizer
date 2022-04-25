#pragma once
#include "ShaderProgram.h"

namespace GL
{
	class PostProcessingShader : public ShaderProgram
	{
	public:
		PostProcessingShader();
		virtual ~PostProcessingShader();

		// Inherited via ShaderProgram
		virtual void getAllUniformLocations() override;
		virtual void bindAttributes() override;
		void loadTime(float time);
		void loadTexture();
	private:
		int location_texture;
		int location_time;
		int location_overlay;
		int location_text;
		const char* VERT_FILE = "postVertex.shader";
		const char* FRAG_FILE = "postFragment.shader";
	};
}