#include "renderer.h"
#include <iostream>

void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char *function, const char* file, int line)
{
	if (GLenum error = glGetError())		//GL_NO_ERROR is 0
	{
		std::cout << "GL Error " << error << " in " << function << " " << file << ": " << line << std::endl;
		return false;
	}
	else
		return true;
}
