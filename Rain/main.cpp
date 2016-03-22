#include "GL/glew.h"
#define GLFW_DLL
#include "GLFW/glfw3.h"

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <vector>

GLuint createProgram(GLchar *, GLchar *, GLchar *);
bool loadObjIndexed(const char * obj_file_name, std::vector<GLfloat>& vertex_position, std::vector<GLfloat>& vertex_normal, std::vector<GLushort>& vertex_indices);

int main (void) {

	if (!glfwInit()) {
		fprintf (stderr, "ERROR: openGL failed to initialize");
		return 1;
	}

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Rain", NULL, NULL);

	if (!window) {
		fprintf (stderr, "ERROR: openGL failed to create window");
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);

	std::vector<GLfloat> monkey_vertices;
	std::vector<GLfloat> monkey_normals;
	std::vector<GLushort> monkey_indices;

	loadObjIndexed("monkey.obj", monkey_vertices, monkey_normals, monkey_indices);

	static const GLfloat rain_colors[] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f
	};

	static const GLfloat rain_positions[] = {
		1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	
	//Create a vertex attribute object
	GLuint rain_vao = 0;
	glGenVertexArrays(1, &rain_vao);
	glBindVertexArray(rain_vao);

	//Create a buffer from openGL
	GLuint rain_vbo = 1;
	glGenBuffers(1, &rain_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, rain_vbo);

	//Allocate space from openGL for the rain attributes
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * monkey_vertices.size() + sizeof(rain_colors) + sizeof(rain_positions), NULL, GL_STATIC_DRAW);

	GLuint offset = 0;
	//Fill each attribue with data
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(GLfloat) * monkey_vertices.size(), monkey_vertices.data());
	offset += sizeof(GLfloat) * monkey_vertices.size();

	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(rain_colors), rain_colors);
	offset += sizeof(rain_colors);

	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(rain_positions), rain_positions);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(GLfloat) * monkey_vertices.size()));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid *)(sizeof(GLfloat) * monkey_vertices.size() + sizeof(rain_colors)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);

	GLuint vertex_indices_buffer;
	glGenBuffers(1, &vertex_indices_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_indices_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * monkey_indices.size(), monkey_indices.data(), GL_STATIC_DRAW);

	GLuint rain_program = createProgram("rain.vert", "rain.frag", "rain.geom");

	glm::mat4 model_matrix (1.0f);//, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);

	GLfloat light_position[] = {0.3f, 0.0f, 0.0f, 1.0f};
	GLfloat light_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat light_power = 100.0f;
	GLfloat position_offset[] = {0.0f, 0.0f, 0.0f, 0.0f};

	glfwSetTime(0);
	float time;

	while (!glfwWindowShouldClose (window)) {
		time = glfwGetTime();

		position_offset[1] = 1.0f * fmodf(time, 10.0f) - 6.0f;

		//Rotating loot-at matrix
		glm::vec3 camera_eye	(0.3f, 0.0f, 1.0f);
		glm::vec3 camera_stage	(0.3f, 0.0f, 0.0f);
		glm::vec3 camera_up		(0.0f, -1.0f, 0.0f);

		glm::mat4 perspective_matrix = glm::perspective(30.0f,  16.0f/9.0f, 0.1f, 100.0f);
		glm::mat4 view_matrix = glm::lookAt(camera_eye, camera_stage, camera_up);

		glm::mat4 mvp_matrix = perspective_matrix * view_matrix * model_matrix;

		glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram (rain_program);


		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertex_indices_buffer);
		glBindVertexArray (rain_vao);

		glUniformMatrix4fv(4, 1, GL_FALSE, &mvp_matrix[0][0]);
		glUniform4f(5, light_position[0], light_position[1], light_position[2], light_position[3]);
		glUniform4f(6, light_color[0], light_color[1], light_color[2], light_color[3]);
		glUniform1f(7, light_power);
		glUniform4f(8, position_offset[0], position_offset[1], position_offset[2], position_offset[3]);

		glDrawElementsInstanced (GL_TRIANGLES, monkey_indices.size(), GL_UNSIGNED_SHORT, (void *)0, 4);

		glfwPollEvents ();

		glfwSwapBuffers (window);
	}

	glDeleteProgram(rain_program);
	glDeleteVertexArrays(1, &rain_vao);
	glDeleteBuffers(1, &rain_vbo);
}


GLuint createProgram(GLchar * vert_shader, GLchar * frag_shader, GLchar * geom_shader) {
	std::string vs_code;
	std::ifstream vs_file(vert_shader);

	if (vs_file.is_open()) {
		std::string line = "";

		while (getline (vs_file, line))
			vs_code += "\n" + line;

		vs_file.close();
	} else {
		fprintf(stderr, "ERROR: %s could not be opened", vert_shader);
		return 1;
	}

	std::string fs_code;
	std::ifstream fs_file(frag_shader);

	if (fs_file.is_open()) {
		std::string line = "";

		while (getline (fs_file, line))
			fs_code += "\n" + line;

		fs_file.close();
	} else {
		fprintf(stderr, "ERROR: %s could not be opened", frag_shader);
		return 1;
	}

	std::string gs_code;
	std::ifstream gs_file(geom_shader);

	if (gs_file.is_open()) {
		std::string line = "";

		while (getline (gs_file, line))
			gs_code += "\n" + line;

		gs_file.close();
	} else {
		fprintf(stderr, "ERROR: %s could not be opened", geom_shader);
		return 1;
	}
	
	//Vertex Shader
	char const* vs_pointer = vs_code.c_str();
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vs_pointer, NULL);
	glCompileShader(vs);

	GLint vs_log_length;
	glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &vs_log_length);

	if (vs_log_length > 1) {
		GLchar* vs_log = new char[vs_log_length + 1];
		glGetShaderInfoLog(vs, vs_log_length, NULL, vs_log);
		printf("%s", vs_log);
	}

	//Geometry Shader
	char const* gs_pointer = gs_code.c_str();
	GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(gs, 1, &gs_pointer, NULL);
	glCompileShader(gs);

	GLint gs_log_length;
	glGetShaderiv(gs, GL_INFO_LOG_LENGTH, &gs_log_length);

	if (gs_log_length > 1) {
		GLchar * gs_log = new char[gs_log_length + 1];
		glGetShaderInfoLog(gs, gs_log_length, NULL, gs_log);
		printf("%s", gs_log);
	}

	//Fragment Shader
	char const* fs_pointer = fs_code.c_str();
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fs_pointer, NULL);
	glCompileShader(fs);

	GLint fs_log_length;
	glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &fs_log_length);

	if (fs_log_length > 1) {
		GLchar * fs_log = new char[fs_log_length + 1];
		glGetShaderInfoLog(fs, fs_log_length, NULL, fs_log);
		printf("%s", fs_log);
	}

	//Compile Program
	GLuint program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, gs);
	glAttachShader(program, fs);
	glLinkProgram(program);

	GLint program_log_length;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &program_log_length);

	if (program_log_length > 1) {
		GLchar* program_log = new char[program_log_length + 1];
		glGetProgramInfoLog(program, program_log_length, NULL, program_log);
		printf("%s", program_log);
	}

	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);

	return program;
}

bool loadObjIndexed(const char * obj_file_name, std::vector<GLfloat>& vertex_position, std::vector<GLfloat>& vertex_normal, std::vector<GLushort>& vertex_indices) {
	
	FILE * obj_file = fopen (obj_file_name, "r");
	
	//File not opened
	if ( obj_file == NULL ) {
		fprintf(stderr, "ERROR: %s could not be opened", obj_file_name);

		return false;
	}
	//File opened
	else {
		while ( 1 ) {

			char lineHeader[128];
			int res = fscanf ( obj_file, "%s", lineHeader );
			if ( res == EOF )
				break;

			if ( strcmp (lineHeader, "o") == 0 ) {			//new object
				printf("object\n");
				continue;
			}

			else if ( strcmp (lineHeader, "v") == 0 ) {	//new vertex
				GLfloat x, y, z;
				fscanf(obj_file, "%f %f %f\n", &x, &y, &z);
				vertex_position.push_back(x);
				vertex_position.push_back(y);
				vertex_position.push_back(z);
				vertex_position.push_back(1.0f);
				printf("Vertex position %f %f %f\n", x, y, z);
			}

			else if ( strcmp (lineHeader, "vn") == 0 ) { //new vertex normal
				GLfloat x, y, z;
				fscanf(obj_file, "%f %f %f\n", &x, &y, &z);
				vertex_normal.push_back(x);
				vertex_normal.push_back(y);
				vertex_normal.push_back(z);
				vertex_normal.push_back(1.0f);
			}
			
			else if ( strcmp (lineHeader, "s") == 0 ) {
				char comment_buffer[128];
				fgets(comment_buffer, 128, obj_file);
			}

			else if ( strcmp (lineHeader, "f") == 0 ) {
				GLuint fv1, fn1, fv2, fn2, fv3, fn3;
				fscanf (obj_file, "%u//%u %u//%u %u//%u\n", &fv1, &fn1, &fv2, &fn2, &fv3, &fn3);
				vertex_indices.push_back((short)fv1 - 1);
				vertex_indices.push_back((short)fv2 - 1);
				vertex_indices.push_back((short)fv3 - 1);
				printf("Vertex indices %u %u %u\n", fv1 - 1, fv2 - 1, fv3 - 1);
			} else {
				char comment_buffer[128];
				fgets(comment_buffer, 128, obj_file);
			}
		}
	}

	return true;
}