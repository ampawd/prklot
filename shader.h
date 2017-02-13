#ifndef SHADER_H
#define SHADER_H

#define GLEW_STATIC

#include <iostream>
#include <string>
#include <fstream>

#include <GL/glew.h>

namespace prklot {
	class Shader
	{
		public:
			//	@param GLint - shader type, 
			//	@param const std::string& path/to/shader
			explicit Shader(GLint, const std::string&);
			std::string getSource() const;
			GLint getType() const;
			GLuint getID() const;

		private:
			std::string source;
			GLint type;
			GLuint ID;

		protected:
	};
};



#endif // SHADER_H
