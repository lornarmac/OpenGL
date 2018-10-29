#include <GL/glew.h>
#include <GLFW/glfw3.h>	// would use EGL with OpenGLES?
//#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"


struct ShaderProgramSource {
	std::string VertexSource;
	std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType {
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1,
	};

	ShaderType type = ShaderType::NONE;

	std::string line;
	std::stringstream ss[2];

	while (getline(stream, line)) {
		if (line.find("#shader") != std::string::npos) {
			if (line.find("vertex") != std::string::npos) {
				// set mode to vertex
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos) {
				// set mode to fragment
				type = ShaderType::FRAGMENT;
			}
		}
		else if (ShaderType::NONE != type) {
			ss[(int)type] << line << '\n';
		}
	}

	return{ ss[0].str(), ss[1].str() };
}

static unsigned int CompileShader(unsigned int type, const std::string& source) {

	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);		//"iv" --> integer vector

	if (GL_FALSE == result) {
		int len;

		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len);
		char *errormsg = (char *)alloca(len * sizeof(char));

		glGetShaderInfoLog(id, len, &len, errormsg);

		std::cout << "Error compiling " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << errormsg << std::endl;

		glDeleteShader(id);
		return 0;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}


int main(void)
{
	GLFWwindow* window;
	float red = 0.0f;
	float green = 0.0f;
	float blue = 0.0f;
	float alpha = 0.0f;
	float increment = 0.0001f;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Explicitly ask for the compatibility profile for OpenGL Context - this is the default anyway */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);	// creates a window AND it's openGL context
	if (!window)
	{
		ASSERT(false);
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	std::cout << glGetString(GL_VERSION) << std::endl;

	if (glewInit() != GLEW_OK) {
		std::cout << "GLEW Error!" << std::endl;
		return -1;
	}

	{	// we're creating a scope here so that the vb and ib which we create on the stack are destroyed before we call glfwTerminate 
		// if we don't enclose them in a scope then they don't get destroyed until after glfwTerminate, at which point there's no 
		// glcontext and glCheckError will return an error if there is no valid opengl context, since we call glCheckError in a loop 
		// the program would never terminate
	float positions[] = {
		-0.5f, -0.5f,		// 0 (vertex 0, position attribute only)
		 0.5f, -0.5f,		// 1	
		 0.5f,  0.5f,		// 2
		-0.5f,  0.5f,		// 3	
	};

	unsigned int indices[] = {		// could use char / unsigned short, BUT it MUST be an unsigned type
		0, 1, 2,
		2, 3, 0
	};

	// create vertex array:
	unsigned int vao;
	GLCall(glGenVertexArrays(1, &vao));
	GLCall(glBindVertexArray(vao));

	VertexArray va;

	// create vertex buffer:
	VertexBuffer vb(positions, 4 * 2 * sizeof(float));

	VertexBufferLayout layout;
	layout.Push<float>(2);
	va.AddBuffer(vb, layout);

	// create index buffer:
	IndexBuffer ib(indices, 6);

	// create shader:
	ShaderProgramSource source = ParseShader("res/shaders/basic.shader");
	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glUseProgram(shader));

	// set up the uniform (variable) used by the shader
	GLCall(int location = glGetUniformLocation(shader, "u_Colour"));
	ASSERT(location != -1);	// note -1 means the uniform isn't used in the program, we can assert because we know it is
	GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

	/* unbind everything - we're doing this to make clear the steps needed each time we do a draw below */
	GLCall(glBindVertexArray(0));
	GLCall(glUseProgram(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/* Do necessary binding before we draw */
		// bind shader:
		GLCall(glUseProgram(shader));
		GLCall(glUniform4f(location, red, green, blue, 1.0f));

		// bind va
		va.Bind();
		// bind index buffer
		ib.Bind();

		// draw
		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));	// can use nullptr because index buffer already bound

		if (red < 1.0)
			red += increment;
		else if (blue < 1.0)
			blue += increment;
		else if (green < 1.0)
			green += increment;


		/* Swap front and back buffers */
		GLCall(glfwSwapBuffers(window));

		/* Poll for and process events */
		GLCall(glfwPollEvents());
	}

	GLCall(glDeleteProgram(shader));

	}
	glfwTerminate();
	return 0;
}

