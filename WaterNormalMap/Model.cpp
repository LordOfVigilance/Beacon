#include "Model.h"


Model::Model(GLchar* fileName) : matrix(1.0f) {
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLushort> indices;

	loadObjIndexed(fileName, vertices, normals, indices);

	//Create a vertex attribute object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	//Create a buffer from openGL
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//Allocate space from openGL for the rain attributes
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), NULL, GL_STATIC_DRAW);

	GLuint offset = 0;
	//Fill each attribue with data
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(GLfloat) * vertices.size(), vertices.data());

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(0);

	glGenBuffers(1, &vertexIndicesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices.size(), indices.data(), GL_STATIC_DRAW);

	vertexCount = indices.size();
}

bool Model::loadObjIndexed(const char * obj_file_name, std::vector<GLfloat>& vertex_position, std::vector<GLfloat>& vertex_normal, std::vector<GLushort>& vertex_indices) {
	
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
			} else {
				char comment_buffer[128];
				fgets(comment_buffer, 128, obj_file);
			}
		}
	}

	return true;
}


Model::~Model(void) {}

void Model::render() {	
	glBindVertexArray (vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndicesBuffer);
	glUniformMatrix4fv(4, 1, GL_FALSE, &mvp[0][0]);

	glDrawElements(GL_TRIANGLES, vertexCount, GL_UNSIGNED_SHORT, (void *) 0);
}

void Model::scale(glm::vec3 vector) {
	matrix = glm::scale(matrix, vector);
}

void Model::translate(glm::vec3 vector) {
	matrix = glm::translate(matrix, vector);
}

void Model::rotate(GLfloat angle, glm::vec3 vector) {
	matrix = glm::rotate(matrix, angle, vector);
}

GLuint Model::getVAO() {
	return vao;
}

GLuint Model::getVBO() {
	return vbo;
}

GLuint Model::getVIB() {
	return vertexIndicesBuffer;
}

GLuint Model::getVertexCount() {
	return vertexCount;
}

glm::mat4x4 Model::getMatrix() {
	return matrix;
}

glm::mat4x4 Model::getMVP() {
	return mvp;
}

void Model::setVP(glm::mat4x4 vp) {
	this->mvp = vp * matrix;
}