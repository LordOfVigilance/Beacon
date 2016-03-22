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

Model::Model(GLchar* fileName, int fileType) : matrix(1.0f) {
	GLuint offset = 0;
	DaeGeom* vertices = new DaeGeom;
	DaeGeom* normals = new DaeGeom;
	DaeGeom* indices = new DaeGeom;

	std::vector<GLfloat> verticesObj;
	std::vector<GLfloat> vertexColorObj;
	std::vector<GLfloat> normalsObj;
	std::vector<GLushort> indicesObj;

	switch(fileType) {
	case COLLADAE:

		loadColladaeIndexed(fileName, vertices, normals, indices);

		//Create a vertex attribute object
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//Create a buffer from openGL
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		//Allocate space from openGL for the rain attributes
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices->size, NULL, GL_STATIC_DRAW);

		//Fill each attribue with data
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(GLfloat) * vertices->size, vertices->data);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(0);

		glGenBuffers(1, &vertexIndicesBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndicesBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indices->size, indices->data, GL_STATIC_DRAW);

		vertexCount = indices->size;

		break;
	case PLY:
		loadPLYIndexed(fileName, verticesObj, normalsObj, indicesObj, vertexColorObj);
		
		//Create a vertex attribute object
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//Create a buffer from openGL
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		//Allocate space from openGL for the rain attributes
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*verticesObj.size() + sizeof(GLushort)*vertexColorObj.size() + sizeof(GLfloat)*normalsObj.size(), NULL, GL_STATIC_DRAW);

		//Fill each attribue with data
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(GLfloat)*verticesObj.size(), verticesObj.data());
		offset += sizeof(GLfloat)*verticesObj.size();
		
		//Fill each attribue with data
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(GLfloat)*vertexColorObj.size(), vertexColorObj.data());
		offset += sizeof(GLfloat)*vertexColorObj.size();
		
		//Fill each attribue with data
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(GLfloat)*normalsObj.size(), normalsObj.data());

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(GLfloat)*verticesObj.size()));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(GLfloat)*verticesObj.size() + sizeof(GLushort)*vertexColorObj.size()));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glGenBuffers(1, &vertexIndicesBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndicesBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort)*indicesObj.size(), indicesObj.data(), GL_STATIC_DRAW);

		vertexCount = indicesObj.size();

		break;
	case WAVEFORM:
	default:
		loadObjIndexed(fileName, verticesObj, normalsObj, indicesObj);

		//Create a vertex attribute object
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		//Create a buffer from openGL
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		//Allocate space from openGL for the rain attributes
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*verticesObj.size(), NULL, GL_STATIC_DRAW);

		//Fill each attribue with data
		glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(GLfloat) * verticesObj.size(), verticesObj.data());

		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);

		glEnableVertexAttribArray(0);

		glGenBuffers(1, &vertexIndicesBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexIndicesBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * indicesObj.size(), indicesObj.data(), GL_STATIC_DRAW);

		vertexCount = indicesObj.size();

		break;
	}
}

bool Model::loadColladaeIndexed(const char * colladaeFileName, DaeGeom* positions, DaeGeom* normals, DaeGeom* indices) {
	tinyxml2::XMLDocument doc;
	doc.LoadFile(colladaeFileName);

	if(doc.Error()) {
		printf("COLLADA ERROR: %s", doc.ErrorName());
		exit(1);

		return false;
	}

	
	doc.RootElement();
	tinyxml2::XMLElement* mesh = doc.FirstChildElement("COLLADA")->FirstChildElement("library_geometries")->FirstChildElement("geometry")->FirstChildElement("mesh");
	tinyxml2::XMLElement* source = mesh->FirstChildElement("source");
	tinyxml2::XMLElement* polyList = mesh->FirstChildElement("polylist");
	tinyxml2::XMLElement* input = polyList->FirstChildElement("input");

	const char* verticesName = input->Attribute("source");
	input = input->NextSiblingElement();
	const char* normalsName = input->Attribute("source");

	tinyxml2::XMLElement* positionsElement = source->FirstChildElement("float_array");
	source->NextSibling();
	tinyxml2::XMLElement* normalsElement = source->FirstChildElement("float_array");
	tinyxml2::XMLElement* indicesElement = polyList->FirstChildElement("p");

	//read the vertices
	int size = -1;
	positionsElement->QueryIntAttribute("count", &size);
	positions->size = size;


	if(positions->size != -1) {
		char* positionText = (char*) positionsElement->GetText();

		positions->data = malloc(positions->size * sizeof(GLfloat));
		((GLfloat*) positions->data)[0] = (GLfloat) atof(strtok(positionText, " "));

		for (int i = 1; i < positions->size; i++) {
			((GLfloat*) positions->data)[i] = (GLfloat) atof(strtok(NULL, " "));
		}
	}

	//read the normals
	/*
	size = -1;
	normalsElement->QueryIntAttribute("count", &size);
	normals->size = size;

	if (normals->size != -1) {
		char* normalsText = (char*) normalsElement->GetText();

		normals->data = malloc(normals->size * sizeof(float));
		((float*) normals->data)[0] = atof(strtok(normalsText, " "));

		for(int i = 1; i < normals->size; i++) {
			((float*) normals->data)[i] = atof(strtok(NULL, " "));	
		}
	}*/

	size = -1;
	polyList->QueryIntAttribute("count", &size);
	indices->size = size;

	//Should access vcount to get size of each polygon instead
	indices->size *= 3;

	if (indices->size > 0) {
		char* indicesText = (char*) indicesElement->GetText();

		indices->data = malloc(indices->size * sizeof(GLushort));

		((GLushort*) indices->data)[0] = (GLushort) atoi(strtok(indicesText, " "));

		for (int i = 1; i < indices->size; i++) {
			((GLushort*) indices->data)[i] = (GLushort) atoi(strtok(NULL, " "));
		}
	}

	return true;
}

bool Model::loadObjIndexed(const char * obj_file_name, std::vector<GLfloat>& vertex_position, std::vector<GLfloat>& vertex_normal, std::vector<GLushort>& vertex_indices) {
	//read the indices
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

bool Model::loadPLYIndexed(const char * objFileName, std::vector<GLfloat>& vertexPositions, std::vector<GLfloat>& vertexNormals, std::vector<GLushort>& vertexIndices, std::vector<GLfloat>& vertexColours) {
	//read the indices
	FILE * objFile = fopen (objFileName, "r");
	
	//File not opened
	if ( objFile == NULL ) {
		fprintf(stderr, "ERROR: %s could not be opened", objFileName);

		return false;
	}
	//File opened
	else {
		printf("PLY\n");

		int vertexCount = 0;
		int faceCount = 0;

		while ( 1 ) {

			char lineHeader[128];
			int res = fscanf ( objFile, "%s", lineHeader );
			if ( res == EOF )
				break;

			if(strcmp (lineHeader, "element") == 0) {
				fscanf ( objFile, "%s", lineHeader );

				if(strcmp(lineHeader, "vertex") == 0) {
					fscanf(objFile, "%u\n", &vertexCount);
				} else if (strcmp(lineHeader, "face") == 0) {
					fscanf(objFile, "%u\n", &faceCount);
				}
			}

			if ( strcmp (lineHeader, "end_header") == 0 ) {
				for(int i=0; i<vertexCount; i++) {
					GLfloat x, y, z, nx, ny, nz;
					GLuint red, green, blue;

					fscanf(objFile, "%f %f %f %f %f %f %u %u %u\n", &x, &y, &z, &nx, &ny, &nz, &red, &green, &blue);
					vertexPositions.push_back(x);
					vertexPositions.push_back(y);
					vertexPositions.push_back(z);
					vertexPositions.push_back(1.0f);

					vertexNormals.push_back(nx);
					vertexNormals.push_back(ny);
					vertexNormals.push_back(nz);

					GLfloat redf, greenf, bluef;
					redf = red/255.0f;
					greenf = green/255.0f;
					bluef = blue/255.0f;
					
					vertexColours.push_back(redf);
					vertexColours.push_back(greenf);
					vertexColours.push_back(bluef);
				}
				
				for(int i=0; i<faceCount; i++) {
					GLchar indexCount;
					GLuint i1, i2, i3;

					fscanf(objFile, "%c %u %u %u\n", &indexCount, &i1, &i2, &i3);
					
					if(indexCount != '3')
						return false;

					vertexIndices.push_back((GLushort)i1);
					vertexIndices.push_back((GLushort)i2);
					vertexIndices.push_back((GLushort)i3);
				}
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