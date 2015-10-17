#include "GL/glew.h"
#define GLFW_DLL
#include "GLFW/glfw3.h"

#include <stdio.h>

int main (void) {

	if(!glfwInit()) {
		fprintf(stderr, "ERROR: Could not start GLFW3\n");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "Game", NULL, NULL);

	if(!window) {
		fprintf(stderr, "ERROR: could not create window with GLFW3\n");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version	= glGetString(GL_VERSION);	// get version string

	printf("Renderer:	%s\n", renderer);
	printf("Version:	%s\n", version);

	glEnable(GL_DEPTH_TEST);							// enable depth testing
	glDepthFunc(GL_LESS);								// set depth function to be less

	



	//Vertex Buffers etc.

	float points[] = {
		-0.5, 0.5, 0.0f,
		 0.5, 0.5, 0.0f,
		-0.5,-0.5, 0.0f,
		 0.5,-0.5, 0.0f
	};

	float points2[] = {
		-0.3f, 0.7f, 0.0f,
		 0.3f, 0.7f, 0.0f,
		-0.3f,-0.7f, 0.0f,
		 0.3f,-0.7f, 0.0f
	};

	GLuint vbo = 0;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), points, GL_STATIC_DRAW);

	GLuint vao = 0;
	glGenVertexArrays (1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	GLuint vbo2 = 1;
	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), points2, GL_STATIC_DRAW);

	GLuint vao2 = 1;
	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);


	//vertex shader

	const char* vertex_shader = 
		"#version 400\n"
		"in vec3 vp;"
		"void main() {"
			"gl_Position = vec4(vp, 1.0f);"
		"}";

	//fragment shader
	const char* fragment_shader = 
		"#version 400\n"
		"out vec4 frag_colour;"
		"void main() {"
			"frag_colour = vec4(0.5f, 0.0f, 0.5f, 1.0f);"
		"}";	

	//fragment shader 2
	const char* fragment_shader_2 = 
		"#version 400\n"
		"out vec4 frag_colour;"
		"void main() {"
			"frag_colour = vec4(0.2f, 0.2f, 0.9f, 1.0f);"
		"}";

	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertex_shader, NULL);
	glCompileShader(vs);

	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragment_shader, NULL);
	glCompileShader(fs);

	GLuint fs2 = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs2, 1, &fragment_shader_2, NULL);
	glCompileShader(fs2);

	GLuint shader_program = glCreateProgram();
	glAttachShader(shader_program, vs);
	glAttachShader(shader_program, fs);
	glLinkProgram(shader_program);

	GLuint shader_program_2 = glCreateProgram();
	glAttachShader(shader_program_2, vs);
	glAttachShader(shader_program_2, fs2);
	glLinkProgram(shader_program_2);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);


	while(!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(shader_program);
		
		glBindVertexArray(vao);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glUseProgram(shader_program_2);

		glBindVertexArray(vao2);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glfwPollEvents();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}