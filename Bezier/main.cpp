#include "GL/GLEW.h"
#define GLFW_DLL
#include "GLFW/glfw3.h"

#include <stdio.h>
#include <string>
#include <fstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

GLuint createProgram (GLchar *, GLchar *, GLchar *, GLchar *, GLchar *);
void keyCallback (GLFWwindow*, int, int, int, int);
std::string readFile (GLchar *);
GLuint createShaderFromCode (GLenum, std::string);
void shaderFromFileAndLink (GLenum, GLchar *, GLuint);

glm::vec2 rotation(0, 0);
bool terminated = false;

int main (void) {
	if (!glfwInit()) {
		fprintf (stderr, "GLFW could not be initialised");
		return 1;
	}

	GLFWwindow * window = glfwCreateWindow (1280, 720, "Bezier Curves", NULL, NULL);

	if (!window) {
		fprintf (stderr, "Window could not be created");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent (window);

	glfwSetKeyCallback (window, keyCallback);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable (GL_DEPTH_TEST);
	glDepthFunc (GL_LESS);

	glPointSize(10.0f);

	//glEnable (GL_CULL_FACE);
	//glCullFace (GL_BACK);

	GLuint bezierProgam = createProgram ("bezier.vert", "bezier.geom", "bezier.tcs", "bezier.tes", "bezier.frag");

	GLfloat control_points[] = {
		0.0f,  0.0f, 0.0f,  1.0f,
		0.0f,  0.2f, 0.33f, 1.0f,
		0.0f,  0.2f, 0.66f, 1.0f,
		0.0f,  0.0f, 1.0f,  1.0f,
		0.33f, 0.0f, 0.0f,  1.0f,
		0.90f, 2.0f, 0.50f, 1.0f,
		0.90f, 2.0f, 0.50f, 1.0f,
		0.33f, 0.0f, 1.0f,  1.0f,
		0.66f, 0.0f, 0.0f,  1.0f,
	   -0.50f, 0.0f, 0.40f, 1.0f,
	   -0.50f, 0.0f, 0.60f, 1.0f,
		0.66f, 0.0f, 1.0f,  1.0f,
		1.0f,  0.0f, 0.0f,  1.0f,
		0.6f,  0.0f, 0.40f, 1.0f,
		0.6f,  0.0f, 0.60f, 1.0f,
		1.0f,  0.0f, 1.0f,  1.0f
	};

	GLuint vao;
	glGenVertexArrays (1, &vao);
	glBindVertexArray (vao);

	GLuint vbo;
	glGenBuffers (1, &vbo);
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof(control_points), control_points, GL_STATIC_DRAW);

	glVertexAttribPointer (0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray (0);

	glm::mat4 model_matrix (1.0);

	glm::vec3 eye	 (0.5f, 0.5f, 10.0f);
	glm::vec3 center (0.5f, 0.5f, -1.0f);
	glm::vec3 up	 (0.0f, -1.0f, 0.0f);

	glm::mat4 view_matrix = glm::lookAt (eye, center, up);
	glm::mat4 mv_matrix = view_matrix * model_matrix;
	glm::mat4 perspective = glm::perspective(50.0f, 16.0f/9.0f, 0.1f, 100.0f);

	int size = sizeof(control_points) / 4.0f / sizeof(float); //16

	while (!glfwWindowShouldClose(window) && !terminated) {
		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		view_matrix = glm::rotate (view_matrix, rotation.x, glm::vec3(0, 1, 0));
		view_matrix = glm::rotate (view_matrix, rotation.y, glm::vec3(1, 0, 0));
		glm::mat4 mv_matrix = view_matrix * model_matrix;

		glUseProgram (bezierProgam);
		glBindVertexArray (vao);

		glUniformMatrix4fv(0, 1, GL_FALSE, &mv_matrix[0][0]);
		glUniformMatrix4fv(1, 1, GL_FALSE, &perspective[0][0]);

		glPatchParameteri (GL_PATCH_VERTICES, 16);
		glDrawArrays (GL_PATCHES, 0, size);

		glfwPollEvents();

		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

void keyCallback (GLFWwindow * window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_S || key == GLFW_KEY_DOWN)
			rotation.y += 0.01f;

		else if (key == GLFW_KEY_W || key == GLFW_KEY_UP)
			rotation.y -= 0.01f;

		else if (key == GLFW_KEY_D || key == GLFW_KEY_RIGHT)
			rotation.x += 0.01f;

		else if (key == GLFW_KEY_A || key == GLFW_KEY_LEFT)
			rotation.x -= 0.01f;

		else if (key ==  GLFW_KEY_ESCAPE)
		{
			glfwTerminate();
			terminated = true;
		}
	}
}

std::string readFile (GLchar * fileName)
{
	std::string fileData;
	std::ifstream file(fileName);

	if (file.is_open()) {
		std::string line = "";

		while (getline (file, line))
			fileData += "\n" + line;

		file.close();
	} else {
		fprintf(stderr, "ERROR: %s could not be opened", fileName);
		glfwTerminate();
	}

	return fileData;
}

GLuint createShaderFromCode (GLenum shaderType, std::string code) {
	char const* source_code_pointer = code.c_str();
	GLuint shaderInt = glCreateShader(shaderType);
	glShaderSource(shaderInt, 1, &source_code_pointer, NULL);
	glCompileShader(shaderInt);

	GLint shaderLogLength;
	glGetShaderiv(shaderInt, GL_INFO_LOG_LENGTH, &shaderLogLength);

	if (shaderLogLength > 1) {
		GLchar* vs_log = new char[shaderLogLength + 1];
		glGetShaderInfoLog(shaderInt, shaderLogLength, NULL, vs_log);
		printf("Shader\n" + shaderType);
		printf("%s", vs_log);
		glfwTerminate();
	}

	return shaderInt;
}

void shaderFromFileAndLink(GLenum shaderType, GLchar * fileName, GLuint program)
{
	std::string code = readFile (fileName);

	GLuint shader = createShaderFromCode(shaderType, code);

	glAttachShader (program, shader);

	glDeleteShader(shader);

	return;
}

GLuint createProgram(GLchar * vert_shader, GLchar * geom_shader, GLchar * tes_control_shader, GLchar * tes_eval_shader, GLchar * frag_shader)
{
	//Compile Program
	GLuint program = glCreateProgram();

	//Vertex Shader
	shaderFromFileAndLink(GL_VERTEX_SHADER, vert_shader, program);

	//shaderFromFileAndLink(GL_GEOMETRY_SHADER, geom_shader, program);

	shaderFromFileAndLink(GL_TESS_CONTROL_SHADER, tes_control_shader, program);

	shaderFromFileAndLink(GL_TESS_EVALUATION_SHADER, tes_eval_shader, program);

	shaderFromFileAndLink(GL_FRAGMENT_SHADER, frag_shader, program);

	glLinkProgram(program);

	GLint program_log_length;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &program_log_length);

	if (program_log_length > 1) {
		GLchar* program_log = new char[program_log_length + 1];
		glGetProgramInfoLog(program, program_log_length, NULL, program_log);
		printf("%s", program_log);
		glfwTerminate();
	}

	return program;
}