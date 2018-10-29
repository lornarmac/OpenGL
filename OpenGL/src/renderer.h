#pragma once

#include <GL/glew.h>

//#define ASSERT(x) if(!(x)) assert(false);
#define ASSERT(x) if(!(x)) __debugbreak();	// note that __ indicates this is a MSVC compiler intrinsic call; won't work with other compilers

#ifdef _DEBUG
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#endif

void GLClearError();

bool GLLogCall(const char *function, const char* file, int line);
