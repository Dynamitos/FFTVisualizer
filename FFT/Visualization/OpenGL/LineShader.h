#pragma once
#include "ShaderProgram.h"

namespace GL
{
	class LineShader : public ShaderProgram
	{
	public:
		LineShader();

		void loadColor(glm::vec4 color);
	protected:
		// Inherited via ShaderProgram
		virtual void getAllUniformLocations() override;
		virtual void bindAttributes() override;

		const char* VERT_FILE = "lineVertex.shader";
		const char* FRAG_FILE = "lineFragment.shader";
	private:
		int location_color;
	};
}