#include "utils.h"
#include "skybox.h"
#include <stdlib.h>


SkyBox::SkyBox(int width, int height, float size, const char *vshaderfile, const char* fshaderfile, GLuint *texCube_ref)
{
	//Create an OpenGL program from the given shader files
	program = createProgram(vshaderfile, fshaderfile);
	getShaderVariables();
	texCube = *texCube_ref;

	//Create geometry
	createGeometry(size);

	//Setup transformations - MVP matrices
	setupMVP(width, height);

	//Set values to the shader values: attributes, and uniform
	setShaderVariables();
}

SkyBox::~SkyBox()
{
	glDeleteProgram(program);
}

void SkyBox::getShaderVariables()
{
	glUseProgram(program);

	//Attribute variables
	vVertex_attrib = glGetAttribLocation(program, "vVertex");
	if(vVertex_attrib == -1) {
		fprintf(stderr, "Could not bind location: vVertex\n");
		exit(0);
	}

	//Uniform variables
	vModel_uniform = glGetUniformLocation(program, "vModel");
	if(vModel_uniform == -1){
		fprintf(stderr, "Could not bind location: vModel\n");
		exit(0);
	}

	vView_uniform = glGetUniformLocation(program, "vView");
	if(vView_uniform == -1){
		fprintf(stderr, "Could not bind location: vView\n");
		exit(0);
	}

	vProjection_uniform = glGetUniformLocation(program, "vProjection");
	if(vProjection_uniform == -1){
		fprintf(stderr, "Could not bind location: vProjection\n");
		exit(0);
	}

	//Sampler variable
	cube_map_uniform =glGetUniformLocation(program, "cube_map");
	if(cube_map_uniform == -1)
	{
		fprintf(stderr, "Could not bind uniform: cube_map\n");
		exit(0);
	}
}

void SkyBox::setShaderVariables()
{
	glUseProgram(program);

	//vVertex is set during VAO creation

	//Uniform variables
	glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT)); 
	glUniformMatrix4fv(vView_uniform, 1, GL_FALSE, glm::value_ptr(viewT)); 
	glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT)); 

	//Sampler variable
	glUniform1i(cube_map_uniform, 0);
}

void SkyBox::createGeometry(float size)
{
	glUseProgram(program);

	//Cube data
	GLfloat cube_vertices[] = {10, 10, 10, -10, 10, 10, -10, -10, 10, 10, -10, 10, //Front
				   10, 10, -10, -10, 10, -10, -10, -10, -10, 10, -10, -10}; //Back
	GLushort cube_indices[] = {0, 3, 2, 0, 2, 1, //Front
				4, 6, 7, 4, 5, 6, //Back
				5, 1, 2, 5, 2, 6, //Left
				4, 7, 3, 4, 3, 0, //Right
				1, 4, 0, 1, 5, 4, //Top
				2, 3, 7, 2, 7, 6}; //Bottom


	//Position information (data + format)
	int nVertices = 6*2*3; //(6 faces) * (2 triangles each) * (3 vertices each)
	GLfloat *expanded_vertices = new GLfloat[nVertices*3];
	for(int i=0; i<nVertices; i++) {
		expanded_vertices[i*3] = cube_vertices[cube_indices[i]*3];
		expanded_vertices[i*3 + 1] = cube_vertices[cube_indices[i]*3+1];
		expanded_vertices[i*3 + 2] = cube_vertices[cube_indices[i]*3+2];
	}

	//Create VAO and VBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &vertex_VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
	glBufferData(GL_ARRAY_BUFFER, nVertices*3*sizeof(GLfloat), expanded_vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(vVertex_attrib);
	glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
	delete []expanded_vertices;

	glBindVertexArray(0);
}

//Setup Model, View, and Transformation matrices.
void SkyBox::setupMVP(int screen_width, int screen_height)
{
	glUseProgram(program);

	//Modelling transformations (Model -> World coordinates)
	modelT = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));//Model coordinates are the world coordinates

	//Viewing transformations (World -> Camera coordinates
	viewT = glm::lookAt(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, 1.0, 0.0));
	//Camera at (0, 0, 0) looking down the negative Z-axis in a right handed coordinate system

	//Set projection matrix
	setProjectionMatrix(screen_width, screen_height);
}

void SkyBox::render()
{
	glUseProgram(program);
	//glDepthMask(GL_FALSE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texCube);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6*2*3);
	glBindVertexArray(0);
}

//A seperate function for setting up projection matrix since it is also required by the main program during window resize.
void SkyBox::setProjectionMatrix(int width, int height)
{
	glUseProgram(program);
	//Projection transformation (Orthographici/Perspective projection)
	float aspect = (float)width/(float)height;
	float view_height = 100.0f;
	//glm::mat4 projection = glm::ortho(-view_height*aspect/2.0f, view_height*aspect/2.0f, -view_height/2.0f, view_height/2.0f, 0.1f, 1000.0f);
	projectionT = glm::perspective(45.0f, aspect, 0.1f, 1000.0f);
	glUniformMatrix4fv(vProjection_uniform, 1, GL_FALSE, glm::value_ptr(projectionT)); 
}

//Roate the skybox to see around the scene on mouse drag
void SkyBox::rotate(const float angle, const glm::vec3 &axis_in_camera_coord)
{
	glUseProgram(program);
	glm::mat3 camera2object = glm::inverse(glm::mat3(viewT*modelT));
	glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;

	modelT = glm::rotate(modelT, angle, axis_in_object_coord);
	glUniformMatrix4fv(vModel_uniform, 1, GL_FALSE, glm::value_ptr(modelT)); 
}
