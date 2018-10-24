#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

//#define ASSERT(x) if(!(x)) assert(false);
#define ASSERT(x) if(!(x)) __debugbreak();	// note that __ indicates this is a MSVC compiler intrinsic call; won't work with other compilers

#ifdef _DEBUG
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))
#else
#define GLCall(x) x
#endif

static void GLClearError()
{
	while (glGetError() != GL_NO_ERROR);
}

static bool GLLogCall(const char *function, const char* file, int line)
{
	if (GLenum error = glGetError())		//GL_NO_ERROR is 0
	{
		std::cout << "GL Error " << error << " in " << function << " " << file << ": " << line << std::endl;
		return false;
	}
	else
		return true;
}

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

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Explicitly ask for the compatibility profile for OpenGL Context - this is the default anyway */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

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
	float positions[] = {
		-0.5f, -0.5f,		// 0 (vertex 0, position attribute only)
		0.5f, -0.5f,		// 1	
		0.5f, 0.5f,			// 2
		-0.5f, 0.5f,		// 3	

	};

	unsigned int indices[] = {		// could use char / unsigned short, BUT it MUST be an unsigned type
		0, 1, 2,
		2, 3, 0
	};

	unsigned int vao;
	glGenVertexArrays(1, &vao);

	unsigned int bufferid;
	GLCall(glGenBuffers(1, &bufferid));	// get a buffer id
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, bufferid));		// select the buffer
	GLCall(glBufferData(GL_ARRAY_BUFFER, 2 * 4 * sizeof(float), positions, GL_STATIC_DRAW));	// assign data to the buffer

	GLCall(glEnableVertexAttribArray(vao));
	// tell opengl the layout of the buffer:
	GLCall(glVertexAttribPointer(vao, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));	// **2nd param of 2 indicates that the data is a vec2

	unsigned int ibo;	// index buffer object
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));		// select the buffer
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW));

	ShaderProgramSource source = ParseShader("res/shaders/basic.shader");
	std::cout << "VERTEX shader" << std::endl;
	std::cout << source.VertexSource << std::endl;
	std::cout << "FRAGMENT shader" << std::endl;
	std::cout << source.FragmentSource << std::endl;

	unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
	GLCall(glUseProgram(shader));

	GLCall(int location = glGetUniformLocation(shader, "u_Colour"));
	ASSERT(location != -1);	// note -1 means the uniform isn't used in the program, we can assert because we know it is

	GLCall(glUniform4f(location, 0.2f, 0.3f, 0.8f, 1.0f));

	float red = 0.0f;
	float green = 0.0f;
	float blue = 0.0f;
	float alpha = 0.0f;

	float increment = 0.0001f;

	/* unbind everything - we're doing this to make clear the steps needed each time we do a draw below */
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/* Do necessary binding before we draw */
		GLCall(glUseProgram(shader));
		GLCall(glUniform4f(location, red, green, blue, 1.0f));

		//GLCall(glBindVertexArray(vao));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, bufferid));
		GLCall(glEnableVertexAttribArray(0));
		// tell opengl the layout of the buffer:
		GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));	// **2nd param of 2 indicates that the data is a vec2		
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));

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

	glfwTerminate();
	return 0;
}

