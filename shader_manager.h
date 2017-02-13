#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "shader.h"

namespace parking_task {
	class ShaderManager
	{
	public:
		ShaderManager();
		GLuint buildProgram(const Shader&, const Shader&);
		GLuint buildProgram(const Shader&, const Shader&, const Shader&);
		void use(GLuint);
		GLuint getUsingProgram() const;

	private:
		GLuint currentShaderProgram;
		char errorMessage[1024];
		GLint compileShader(const Shader&);
		void showShaderInfoLog(const Shader&);
		void showProgramInfoLog(GLuint);

	protected:

	};
};

#endif // SHADER_MANAGER_H
